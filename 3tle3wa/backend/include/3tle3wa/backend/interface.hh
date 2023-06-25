#pragma once

#include <string>
#include <string_view>

#include "3tle3wa/utils/Logs.hh"

class serializable {
   private:
    char *buf_{nullptr};
    size_t buflen_{0};
    FILE *fp_{nullptr};

   protected:
    virtual void format_str(FILE *fp) = 0;

   public:
    virtual std::string to_string() final;

    virtual std::string_view to_sv() final;

    virtual const char *c_str() final;

    virtual void make_str() final;

    ~serializable();
};