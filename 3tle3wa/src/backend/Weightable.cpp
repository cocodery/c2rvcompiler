#include "3tle3wa/backend/Interface.hh"

bool Weightable::operator>(const Weightable &other) { return this->weight_ > other.weight_; }

bool Weightable::operator==(const Weightable &other) { return this->weight_ == other.weight_; }

bool Weightable::operator<(const Weightable &other) { return this->weight_ < other.weight_; }