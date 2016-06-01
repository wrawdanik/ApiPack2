#!/usr/bin/env python

# Utility for generating sample data dump of all related objects referenced by schedule events for a given set of lineups.



import argparse
import sys
import zlib
import datetime
import re
import time
import os
import subprocess

import pg8000

sys.path.append("../../thrift/gen-py")

# import thrift libraries
from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TCompactProtocol

from datetime import date

# import thrift generated python module for metamorphosisDataModel

from gracenote.metamorphosis.data.core.lineup.ttypes import *
from gracenote.metamorphosis.data.core.station.ttypes import *
from gracenote.metamorphosis.data.core.affiliation.ttypes import *
from gracenote.metamorphosis.data.core.scheduleDay.ttypes import *
from gracenote.metamorphosis.data.core.image.ttypes import *
from gracenote.metamorphosis.data.core.person.ttypes import *
from gracenote.metamorphosis.data.core.team.ttypes import *
from gracenote.metamorphosis.data.core.university.ttypes import *
from gracenote.metamorphosis.data.core.conference.ttypes import *
from programTMS.ttypes import *

from subprocess import Popen, PIPE, STDOUT


selectObjectSql='SELECT  id,thrift FROM catapult.general where topic=? and id IN (%s)';

#copyObjectSq'\copy catapult.schedule_day FROM '/home/warmi/Development/git/ApiPack2/local/sampleData/schedules.dump' DELIMITER ';' CSV

def chunkedList(list, count):
    count = max(1, count)
    return [list[i:i + count] for i in range(0, len(list), count)]

def exportRecordsToFile(connectionData,topic, ids,exportSqlFile):
  
    chunkedIds=chunkedList(list(ids[topic]),512)
    
    print "Exporting SQL for topic "+topic+" for "+str(len(ids[topic]))+" objects."
    
    fileCount=0; 
    for idList in chunkedIds:
      if topic=='metamorphosis.schedule.day':
	sqlCommand="SELECT  * FROM catapult.schedule_day"
	sqlCommand+=' where schedule_day_id IN (%s)'% ', '.join(["%s" % id for id in idList])
      else:
 	sqlCommand="SELECT *  FROM catapult.general where topic='"+topic+"'"
	sqlCommand+=' and id IN (%s)'% ', '.join(["'%s'" % id for id in idList])     
      exportSqlFile.write('\copy (')
      exportSqlFile.write(sqlCommand)
      exportSqlFile.write(") To 'sampleData/export_data_"+topic+"_"+str(fileCount)+".dump' WITH CSV; \n")
      fileCount=fileCount+1


def createExportScript(connectionData,sqlCommandFile):
    
    shellScript = open("runExportData.sh", "w+") 
    shellScript.write("#!/bin/bash\n")   
    shellScript.write('SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"\n')
    shellScript.write("cd $SCRIPT_DIR\n")
    shellScript.write("export PGPASSWORD=${1}\n")
    shellScript.write("rm -rf sampleData\n")
    shellScript.write("mkdir sampleData\n")
    shellScript.write('psql -h '+connectionData["dbHost"]+' -d db_metamorphosis -U '+connectionData["dbUser"]+'  -a -w -f '+sqlCommandFile+';\n')
    shellScript.close
    
    os.system("chmod +x runExportData.sh");
    
def handleSchedules(exportedIds,connection,daysBack,daysForward):    
  
    currentDate=datetime.datetime.utcnow()

    requestStartDate = currentDate- datetime.timedelta(days=daysBack)
    requestStartDate=requestStartDate.replace(minute=0, hour=0, second=0, microsecond=0)

    requestEndDate = currentDate+ datetime.timedelta(days=daysForward)
    requestEndDate=requestEndDate.replace(minute=0, hour=0, second=0, microsecond=0)
  
    delta = requestEndDate - requestStartDate
    totalExpected=delta.days*len(exportedIds["metamorphosis.station"])
    totalByteSize=0
    totalCompressedSize=0
    totalEvents=0

    print "processing schedules for "+str(len(exportedIds["metamorphosis.station"]))+" stations from "+str(requestStartDate)+" to "+str(requestEndDate)+" ..."
    
    chunkedIds=chunkedList(list(exportedIds["metamorphosis.station"]),1000)
    for idList in chunkedIds:
	cursor = connection.cursor()	
	sqlCommand="SELECT  schedule_day_id,thrift FROM catapult.schedule_day where day>=? and day<? "
	sqlCommand+=' and station IN (%s)'% ', '.join(["%s" % id for id in idList])	
	
	#print sqlCommand
	
	cursor.execute(sqlCommand,[requestStartDate,requestEndDate])    
    
	missingScheduleDay=0
	
    
 	for row in cursor:
	    transportIn = TTransport.TMemoryBuffer(row[1])
	    protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	    scheduleDay = ScheduleDayData()
	    scheduleDay.read(protocolIn)
	    
	    transportOut = TTransport.TMemoryBuffer()
	    protocolOut = TCompactProtocol.TCompactProtocol(transportOut)
	    scheduleDay.write(protocolOut)
	    compactRow = transportOut.getvalue()
	    
	    if scheduleDay.object is None:
		missingScheduleDay+=1
	    else:
	    	if len(scheduleDay.object.events) >0:
			for event in scheduleDay.object.events:
		    		exportedIds["metamorphosis.program.tmsId"].add(event.remoteId)
				totalEvents+=1		

	      	exportedIds["metamorphosis.schedule.day"].add(str(scheduleDay.object.id))
	      	totalByteSize+=len(row[1])
	      	totalCompressedSize+=len(compactRow)
	    if len(exportedIds["metamorphosis.schedule.day"]) % 1000 == 0:  
		print "\tprocesed "+str(len( exportedIds["metamorphosis.schedule.day"]))+" schedule days out of "+str(totalExpected)+" days. "+str(missingScheduleDay)+" were missing ..."
		print "\ttotal byte size "+str(totalByteSize) + " average schedule day size "+str(totalByteSize/len(exportedIds["metamorphosis.schedule.day"]))
		print "\ttotal compressed byte size "+str(totalCompressedSize) + " average schedule day size "+str(totalCompressedSize/len(exportedIds["metamorphosis.schedule.day"]))
	      
	cursor.close()   
	
    print "Done.Total procesed "+str(len( exportedIds["metamorphosis.schedule.day"]))+" schedule days with "+str(totalEvents)+" events , out of which "+str(missingScheduleDay)+" were missing ..."

def handleFullCollection(exportedIds,connection,topic):
    
    print "processing all "+topic+" ..."
    cursor = connection.cursor()	
    sqlCommand="SELECT  id FROM catapult.general where topic='"+topic+"'"
	
    cursor.execute(sqlCommand)
    for row in cursor:
	exportedIds[topic].add(str(row[0]))
    cursor.close()

def handleTeams(exportedIds,connection):
    print "processing all metamorphosis.team ..."

    sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.team'"	
    cursor = connection.cursor()
    cursor.execute(sqlCommand)

    for row in cursor:
	   transportIn = TTransport.TMemoryBuffer(row[1])
	   protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	   team = TeamData()
	   team.read(protocolIn)
	   exportedIds["metamorphosis.team"].add(str(row[0]))    
	   if len(team.object.brands) >0:	
		for brand in team.object.brands:
			if brand.images!=None:
				if len(brand.images) >0:
					for image in brand.images:
	   					exportedIds["metamorphosis.image"].add(str(image))
    cursor.close()

def handleConferences(exportedIds,connection):
    print "processing all metamorphosis.conference ..."   

    sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.conference'"	
    cursor = connection.cursor()
    cursor.execute(sqlCommand)

    for row in cursor:
	   transportIn = TTransport.TMemoryBuffer(row[1])
	   protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	   conference = ConferenceData()
	   conference.read(protocolIn)	
	   exportedIds["metamorphosis.conference"].add(str(row[0]))
	   if conference.object!=None:
	   	if conference.object.images!=None:      
	   		if len(conference.object.images) >0:	
				for image in conference.object.images:
	   				exportedIds["metamorphosis.image"].add(str(image))
    cursor.close()


def handleUniversities(exportedIds,connection):
    print "processing all metamorphosis.university ..."   

    sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.university'"	
    cursor = connection.cursor()
    cursor.execute(sqlCommand)

    for row in cursor:
	   transportIn = TTransport.TMemoryBuffer(row[1])
	   protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	   university = UniversityData()
	   university.read(protocolIn)	
	   exportedIds["metamorphosis.university"].add(str(row[0])) 
	   if university.object!=None:    
	   	if len(university.object.images) >0:	
			for image in university.object.images:
	   			exportedIds["metamorphosis.image"].add(str(image))
    cursor.close()


def handleAffiliates(exportedIds,connection):
    
  
    print "processing all metamorphosis.affiliation ..."

    sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.affiliation'"	
    cursor = connection.cursor()
    cursor.execute(sqlCommand)

    for row in cursor:
	   transportIn = TTransport.TMemoryBuffer(row[1])
	   protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	   affiliation = AffiliationData()
	   affiliation.read(protocolIn)	
	   exportedIds["metamorphosis.affiliation"].add(str(row[0]))    
	   if affiliation.object.affiliationStationId!=None:
	   	exportedIds["metamorphosis.station"].add(str(affiliation.object.affiliationStationId))
    cursor.close()

def handleStations(exportedIds,connection):
  
    print "processing "+str(len(exportedIds["metamorphosis.station"]))+" stations ..."
  
    chunkedIds=chunkedList(list(exportedIds["metamorphosis.station"]),1000)
    for idList in chunkedIds:
	cursor = connection.cursor()	
	sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.station'"
	sqlCommand+=' and id IN (%s)'% ', '.join(["'%s'" % id for id in idList])	
	
	cursor.execute(sqlCommand)

	for row in cursor:
	    transportIn = TTransport.TMemoryBuffer(row[1])
	    protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	    station = StationData()
	    station.read(protocolIn)	    
	    if len(station.object.logos) >0:	
		for imageId in station.object.logos:
		    exportedIds["metamorphosis.image"].add(str(imageId))
	cursor.close()

def handlePerson(exportedIds,connection):
  
    print "processing "+str(len(exportedIds["metamorphosis.person"]))+" persons ..."
  
    personCount=0
    totalByteSize=0

    chunkedIds=chunkedList(list(exportedIds["metamorphosis.person"]),1000)
    for idList in chunkedIds:
	cursor = connection.cursor()	
	sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.person'"
	sqlCommand+=' and id IN (%s)'% ', '.join(["'%s'" % id for id in idList])	
	
	cursor.execute(sqlCommand)

	for row in cursor:
	    transportIn = TTransport.TMemoryBuffer(row[1])
	    protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	    person = PersonData()
	    person.read(protocolIn)	  
        personCount+=1
        totalByteSize+=len(row[1])
        if len(person.object.images) >0:
		    for image in person.object.images:
		        exportedIds["metamorphosis.image"].add(str(image))
        if len(person.object.awards) >0:	
    		for award in person.object.awards:
    		    exportedIds["metamorphosis.program.tmsId"].add(str(award.program))

		if personCount % 1000 == 0:  
			print "\tprocesed "+str(personCount)+" persons  ..."
			print "\ttotal byte size "+str(totalByteSize) + " average person size "+str(totalByteSize/personCount)

	cursor.close()

def handlePrograms(exportedIds,connection):
  
    print "processing "+str(len(exportedIds["metamorphosis.program.tmsId"]))+" programs ..."
  
    programCount=0
    initialProgramCount=len(exportedIds["metamorphosis.program.tmsId"])

    chunkedIds=chunkedList(list(exportedIds["metamorphosis.program.tmsId"]),1000)
    for idList in chunkedIds:
        cursor = connection.cursor()	
        sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.program.tmsId'"
        sqlCommand+=' and id IN (%s)'% ', '.join(["'%s'" % id for id in idList])	

        totalByteSize=0

        cursor.execute(sqlCommand)

        for row in cursor:
            transportIn = TTransport.TMemoryBuffer(row[1])
            protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
            program = ProgramTMSData()
            program.read(protocolIn)	
            programCount+=1	    
            totalByteSize+=len(row[1])
            if len(program.object.images) >0:
                for image in program.object.images:
                    exportedIds["metamorphosis.image"].add(str(image))
            if len(program.object.relationships) >0:
                for relationship in program.object.relationships:
                    exportedIds["metamorphosis.program.tmsId"].add(str(relationship.id))
            if len(program.object.credits) >0:
                for credit in program.object.credits:
                    exportedIds["metamorphosis.person"].add(str(credit.person.personId))

        if programCount % 1000 == 0:  
            print "\tprocesed "+str(programCount)+" programs  ..."
            print "\ttotal byte size "+str(totalByteSize) + " average program size "+str(totalByteSize/programCount)
			

	cursor.close()

    print "processed total of "+str(programCount)+" programs out of requested "+ str(initialProgramCount)+" programs ..."			

def handleLineups(exportedIds,connection,lineupIds):
    
    print "processing "+str(len(lineupIds))+" lineups ..."
    
    cursor = connection.cursor()		 
    cursor.execute(selectObjectSql% ','.join('?' for i in lineupIds),['metamorphosis.lineup']+lineupIds)

    for row in cursor:
	transportIn = TTransport.TMemoryBuffer(row[1])
        protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
        lineup = LineupData()
        lineup.read(protocolIn)
        exportedIds["metamorphosis.lineup"].add(lineup.object.id)
        for channelMap in lineup.object.channels:
	    exportedIds["metamorphosis.station"].add(str(channelMap.stationId))
	exportedIds["metamorphosis.headend"].add(lineup.object.headendId)
 
    cursor.close()


parser = argparse.ArgumentParser()
parser.add_argument('-l', '--lineupIds', help='List of comma separated lineup ids ( USA-NY64781-X,CAN-0009050-X)')
parser.add_argument('-db', '--database',required=True, help='Connection string to the PostgreSQL catapult schema database in the form of user:password@host:port')
parser.add_argument('-p', '--daysPast', help='How many schedule days in the past. ',type=int, default=7)
parser.add_argument('-f', '--daysForward', help='How many schedule days in the future. ',type=int, default=30)




args = parser.parse_args()


lineupIds = []

if args.lineupIds is not None:
    lineupIds = [x.strip() for x in args.lineupIds.split(",") if x]

connectionData={}

connectionData["dbHost"]=""
connectionData["dbPassword"]=""
connectionData["dbUser"]=""
connectionData["dbPort"]=0


try:
    connectionData["dbUser"] = (args.database.split("@"))[0].split(":")[0]
    connectionData["dbPassword"] = (args.database.split("@"))[0].split(":")[1]
    connectionData["dbHost"] = (args.database.split("@"))[1].split(":")[0]
    if len((args.database.split("@"))[1].split(":"))>1:
      connectionData["dbPort"] = int((args.database.split("@"))[1].split(":")[1])
except:
    print "Error parsing database connection string " + args.database
    print "Expected format user:password@host:port"
    sys.exit(1)

try:
    connection = pg8000.connect(user=connectionData["dbUser"], host=connectionData["dbHost"], port=connectionData["dbPort"], database="db_metamorphosis", password=connectionData["dbPassword"])
    pg8000.paramstyle = "qmark"
except pg8000.InterfaceError, e:
    print "Error: Unable to connect to PostgreSQL instance at " + args.database
    print "Error code :" + str(e)
    sys.exit(1)
    
print "Sucesfully connnected to database "+connectionData["dbHost"]+"."   
    
print "Running prepare job for "+str(len(lineupIds))+" lineups.\n\n"   

start = time.time()

exportSqlFile = open('export_data.sql', "w+")    

exportedIds={}

exportedIds["metamorphosis.station"]=set()
exportedIds["metamorphosis.lineup"]=set()
exportedIds["metamorphosis.image"]=set()
exportedIds["metamorphosis.person"]=set()
exportedIds["metamorphosis.program.tmsId"]=set()
exportedIds["metamorphosis.affiliation"]=set()
exportedIds["metamorphosis.market"]=set()
exportedIds["metamorphosis.schedule.day"]=set()
exportedIds["metamorphosis.provider"]=set()
exportedIds["metamorphosis.headend"]=set()
exportedIds["metamorphosis.rating.body"]=set()
exportedIds["metamorphosis.rating.value"]=set()
exportedIds["metamorphosis.animation"]=set()
exportedIds["metamorphosis.award.type"]=set()
exportedIds["metamorphosis.award.body"]=set()
exportedIds["metamorphosis.genre"]=set()
exportedIds["metamorphosis.production.company"]=set()
exportedIds["metamorphosis.advisory"]=set()
exportedIds["metamorphosis.holiday"]=set()
exportedIds["metamorphosis.playoff"]=set()
exportedIds["metamorphosis.team"]=set()
exportedIds["metamorphosis.venue"]=set()
exportedIds["metamorphosis.warning"]=set()
exportedIds["metamorphosis.conference"]=set()
exportedIds["metamorphosis.university"]=set()

handleFullCollection(exportedIds,connection,"metamorphosis.rating.body")
handleFullCollection(exportedIds,connection,"metamorphosis.rating.value")
handleFullCollection(exportedIds,connection,"metamorphosis.animation")
handleFullCollection(exportedIds,connection,"metamorphosis.award.type")
handleFullCollection(exportedIds,connection,"metamorphosis.award.body")
handleFullCollection(exportedIds,connection,"metamorphosis.market")
handleFullCollection(exportedIds,connection,"metamorphosis.provider")
handleFullCollection(exportedIds,connection,"metamorphosis.genre")
handleFullCollection(exportedIds,connection,"metamorphosis.production.company")
handleFullCollection(exportedIds,connection,"metamorphosis.advisory")
handleFullCollection(exportedIds,connection,"metamorphosis.holiday")
handleFullCollection(exportedIds,connection,"metamorphosis.playoff")
handleFullCollection(exportedIds,connection,"metamorphosis.venue")
handleFullCollection(exportedIds,connection,"metamorphosis.warning")

handleAffiliates(exportedIds,connection)
handleTeams(exportedIds,connection)
handleUniversities(exportedIds,connection)
handleConferences(exportedIds,connection)

handleLineups(exportedIds,connection,lineupIds)
handleSchedules(exportedIds,connection,args.daysPast, args.daysForward)
handlePrograms(exportedIds,connection)
handlePerson(exportedIds,connection)
handleStations(exportedIds,connection)


exportRecordsToFile(connectionData,'metamorphosis.lineup',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.station',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.affiliation',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.market',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.schedule.day',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.program.tmsId',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.image',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.provider',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.headend',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.person',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.rating.body',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.rating.value',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.animation',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.award.type',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.award.body',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.genre',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.production.company',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.advisory',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.holiday',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.playoff',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.team',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.venue',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.university',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.warning',exportedIds,exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.conference',exportedIds,exportSqlFile)

createExportScript(connectionData,'export_data.sql')


exportSqlFile.close

end = time.time()
print "\n\nFinished export job in "+str((end - start))+" seconds."
