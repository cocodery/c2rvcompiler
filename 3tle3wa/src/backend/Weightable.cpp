#include "3tle3wa/backend/Interface.hh"

bool Weightable::operator>(const Weightable &other) { return weight_ > other.weight_; }

bool Weightable::operator==(const Weightable &other) { return weight_ == other.weight_; }

bool Weightable::operator<(const Weightable &other) { return weight_ < other.weight_; }

double Weightable::Weight() { return weight_; }

