#include "backend/interface.hh"

//
// serializable
//

void serializable::make_str() {
    if (buf_) {
        free(buf_);
        buf_ = nullptr;
        buflen_ = 0;
    }

    fp_ = open_memstream(&buf_, &buflen_);
    Assert(fp_, "open memstream fail");

    format_str(fp_);

    fflush(fp_);
    fclose(fp_);
    fp_ = nullptr;
}

std::string serializable::to_string() {
    if (buf_ == nullptr) {
        make_str();
    }
    Assert(buf_, "make_str not implemented well");
    return buf_;
}

std::string_view serializable::to_sv() {
    if (buf_ == nullptr) {
        make_str();
    }
    Assert(buf_, "make_str not implemented well");
    return std::string_view(buf_, buflen_);
}

const char *serializable::c_str() {
    if (buf_ == nullptr) {
        make_str();
    }
    Assert(buf_, "make_str not implemented well");
    return buf_;
}

serializable::~serializable() {
    if (buf_) {
        free(buf_);
        buf_ = nullptr;
        buflen_ = 0;
    }
}
