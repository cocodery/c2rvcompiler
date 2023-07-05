#pragma once

#include <string>
#include <string_view>

#include "3tle3wa/utils/Logs.hh"

class InternalTranslation;

class Serializable {
   private:
    char *buf_{nullptr};
    size_t buflen_{0};
    FILE *fp_{nullptr};

   protected:
    virtual void formatString(FILE *fp) = 0;

   public:
    virtual std::string ToString() final;

    virtual std::string_view ToStringView() final;

    virtual const char *CString() final;

    virtual void MakeString() final;

    ~Serializable();
};

class Weightable {
    protected:
     double weight_;

     virtual void calculateWeight() = 0;

    public:
     virtual bool operator>(const Weightable &other) final;

     virtual bool operator==(const Weightable &other) final;
     
     virtual bool operator<(const Weightable &other) final;
};

class Translatable {
    public:
     virtual void TranslateTo(InternalTranslation &itx);
};