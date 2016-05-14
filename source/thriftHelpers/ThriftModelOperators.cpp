//
// Created by warmi on 5/14/16.
//

#ifndef APIPACK2_THRIFTMODELOPERATORS_H_H
#define APIPACK2_THRIFTMODELOPERATORS_H_H

#endif //APIPACK2_THRIFTMODELOPERATORS_H_H


#include "station_types.h"
#include "merckx_types.h"
#include "team_types.h"




bool gracenote::metamorphosis::data::core::Attribute::operator<(const gracenote::metamorphosis::data::core::Attribute &right) const
{
    return this->name<right.name;
}

bool gracenote::metamorphosis::data::core::RelatedStation::operator<(const gracenote::metamorphosis::data::core::RelatedStation &right) const
{
    return this->id<right.id;
}

bool Description::operator<(const Description &right) const
{
    return this->text<right.text;
}

bool Sport::operator<(const Sport &right) const
{
    return this->eid<right.eid;
}

bool Event::operator<(const Event &right) const
{
    return this->eid<right.eid;
}

bool Organization::operator<(const Organization &right) const
{
    return this->eid<right.eid;
}

bool OrganizationType::operator<(const OrganizationType &right) const
{
    return this->eid<right.eid;
}

bool OrganizationSeason::operator<(const OrganizationSeason &right) const
{
    return this->eid<right.eid;
}

bool SportFranchise::operator<(const SportFranchise &right) const
{
    return this->eid<right.eid;
}

bool SportFranchiseSeason::operator<(const SportFranchiseSeason &right) const
{
    return this->eid<right.eid;
}

bool Venue::operator<(const Venue &right) const
{
    return this->eid<right.eid;
}

bool VenueFormerName::operator<(const VenueFormerName &right) const
{
    return this->eid<right.eid;
}

bool VenueCapacity::operator<(const VenueCapacity &right) const
{
    return this->eid<right.eid;
}


bool EventType::operator<(const EventType &right) const
{
    return this->eid<right.eid;
}

bool College::operator<(const College &right) const
{
    return this->eid<right.eid;
}

bool gracenote::metamorphosis::data::core::Country::operator<(const gracenote::metamorphosis::data::core::Country &right) const
{
    return this->numeric<right.numeric;
}

bool gracenote::metamorphosis::data::core::TeamMember::operator<(const gracenote::metamorphosis::data::core::TeamMember &right) const
{
    return this->personNameId<right.personNameId;
}
