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
from gracenote.metamorphosis.service.ttypes import *
from gracenote.metamorphosis.service import MetamorphosisUpdateService

from gracenote.metamorphosis.data.core.lineup.ttypes import *
from gracenote.metamorphosis.data.core.station.ttypes import *
from gracenote.metamorphosis.data.core.affiliation.ttypes import *
from gracenote.metamorphosis.data.core.scheduleDay.ttypes import *
from gracenote.metamorphosis.data.core.image.ttypes import *

from subprocess import Popen, PIPE, STDOUT


selectObjectSql='SELECT  id,thrift FROM catapult.general where topic=? and id IN (%s)';

#copyObjectSq'\copy catapult.schedule_day FROM '/home/warmi/Development/git/ApiPack2/local/sampleData/schedules.dump' DELIMITER ';' CSV

def chunkedList(list, count):
    count = max(1, count)
    return [list[i:i + count] for i in range(0, len(list), count)]

def exportRecordsToFile(connectionData,topic, ids,exportSqlFile):
  
    chunkedIds=chunkedList(list(ids),512)
    
    print "Exporting SQL for topic "+topic+" for "+str(len(ids))+" objects."
    
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
    shellScript.write("rm -rf sampleData/*\n")
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
    totalExpected=delta.days*len(exportedIds["stations"])
    totalByteSize=0
    totalCompressedSize=0
  
    print "processing schedules for "+str(len(exportedIds["stations"]))+" stations from "+str(requestStartDate)+" to "+str(requestEndDate)+" ..."
    
    chunkedIds=chunkedList(list(exportedIds["stations"]),100)
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
	      exportedIds["schedules"].add(str(scheduleDay.object.id))
	      totalByteSize+=len(row[1])
	      totalCompressedSize+=len(compactRow)
	    if len(exportedIds["schedules"]) % 1000 == 0:  
		print "\tprocesed "+str(len( exportedIds["schedules"]))+" schedule days out of "+str(totalExpected)+" days. "+str(missingScheduleDay)+" were missing ..."
		print "\ttotal byte size "+str(totalByteSize) + " average schedule day size "+str(totalByteSize/len(exportedIds["schedules"]))
		print "\ttotal compressed byte size "+str(totalCompressedSize) + " average schedule day size "+str(totalCompressedSize/len(exportedIds["schedules"]))
	      
	cursor.close()   
	
    print "Done.Total procesed "+str(len( exportedIds["schedules"]))+" schedule days out of which "+str(missingScheduleDay)+" were missing ..."

def handleImages(exportedIds,connection):
    
  
    print "processing "+str(len(exportedIds["images"]))+" images ..."
  
    chunkedIds=chunkedList(list(exportedIds["images"]),512)
    for idList in chunkedIds:
	cursor = connection.cursor()	
	sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.image'"
	sqlCommand+=' and id IN (%s)'% ', '.join(["'%s'" % id for id in idList])	
	
	cursor.execute(sqlCommand)

	for row in cursor:
	    transportIn = TTransport.TMemoryBuffer(row[1])
	    protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	    image = ImageData()
	    image.read(protocolIn)	    
	    if image.object.provider!=None:
		exportedIds["providers"].add(str(image.object.provider))
	cursor.close()

def handleAffiliates(exportedIds,connection):
    
  
    print "processing "+str(len(exportedIds["affiliations"]))+" affiliates ..."
  
    chunkedIds=chunkedList(list(exportedIds["affiliations"]),512)
    for idList in chunkedIds:
	cursor = connection.cursor()	
	sqlCommand="SELECT  id,thrift FROM catapult.general where topic='metamorphosis.affiliation'"
	sqlCommand+=' and id IN (%s)'% ', '.join(["'%s'" % id for id in idList])	
	
	cursor.execute(sqlCommand)

	for row in cursor:
	    transportIn = TTransport.TMemoryBuffer(row[1])
	    protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
	    affiliation = AffiliationData()
	    affiliation.read(protocolIn)	    
	    if affiliation.object.affiliationStationId!=None:
		exportedIds["affiliations"].add(str(affiliation.object.affiliationStationId))
	cursor.close()

def handleStations(exportedIds,connection):
  
    print "processing "+str(len(exportedIds["stations"]))+" stations ..."
  
    chunkedIds=chunkedList(list(exportedIds["stations"]),512)
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
	    if station.object.marketId!=None:
		exportedIds["markets"].add(str(station.object.marketId))
	    if len(station.object.affiliation) >0:
		for stationAffiliation in station.object.affiliations:
		    exportedIds["affiliations"].add(str(stationAffiliation.affiliationId))
	    if len(station.object.logos) >0:	
		for imageId in station.object.logos:
		    exportedIds["images"].add(str(imageId))
	cursor.close()

def handleLineups(exportedIds,connection,lineupIds):
    
    print "processing "+str(len(lineupIds))+" lineups ..."
    
    cursor = connection.cursor()		 
    cursor.execute(selectObjectSql% ','.join('?' for i in lineupIds),['metamorphosis.lineup']+lineupIds)

    for row in cursor:
	transportIn = TTransport.TMemoryBuffer(row[1])
        protocolIn = TBinaryProtocol.TBinaryProtocol(transportIn)
        lineup = LineupData()
        lineup.read(protocolIn)
        exportedIds["lineups"].add(lineup.object.id)
        for channelMap in lineup.object.channels:
	    exportedIds["stations"].add(str(channelMap.stationId))
	exportedIds["headends"].add(lineup.object.headendId)
        #print "lineup: "+lineup.object.id+" channel count "+str(len(lineup.object.channels))
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

exportedIds["stations"]=set()
exportedIds["lineups"]=set()
exportedIds["images"]=set()
exportedIds["persons"]=set()
exportedIds["programs"]=set()
exportedIds["affiliations"]=set()
exportedIds["markets"]=set()
exportedIds["schedules"]=set()
exportedIds["providers"]=set()
exportedIds["headends"]=set()

handleLineups(exportedIds,connection,lineupIds)
handleSchedules(exportedIds,connection,args.daysPast, args.daysForward)
handleStations(exportedIds,connection)
handleAffiliates(exportedIds,connection)
handleImages(exportedIds,connection)

exportRecordsToFile(connectionData,'metamorphosis.lineup',exportedIds["lineups"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.station',exportedIds["stations"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.affiliation',exportedIds["affiliations"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.market',exportedIds["markets"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.schedule.day',exportedIds["schedules"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.image',exportedIds["images"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.provider',exportedIds["providers"],exportSqlFile)
exportRecordsToFile(connectionData,'metamorphosis.headend',exportedIds["headends"],exportSqlFile)

createExportScript(connectionData,'export_data.sql')


exportSqlFile.close

end = time.time()
print "\n\nFinished export job in "+str((end - start))+" seconds."
