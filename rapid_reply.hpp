/*

 The MIT License (MIT)

 Copyright (C) 2015 Suryanathan Padmanabhan

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*/

#ifndef __RAPID_REPLY_HPP_INCLUDED__
#define __RAPID_REPLY_HPP_INCLUDED__

#include <iostream>
#include "etcd.hpp"

// JSON PARSER INCLUDES
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace example {
/////////////////////////////////////////////////////////////////////////////

/**
 * @brief An example json reply wrapper. This can be easily replaced with a user
 * defined wrapper as song as it implements a similar interface
 */
class RapidReply {

  public:
    // LIFECYCLE
    RapidReply(const std::string& reply)
      :document_(),
       header_() {
        _Parse(reply);
    }

    RapidReply(
        const std::string& header,
        const std::string& reply)
     :document_(),
      header_(header) {
        _Parse(reply);
    }

    // OPERATIONS
    void Print() const {
        rapidjson::StringBuffer strbuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
        document_.Accept(writer);
        std::cerr << strbuf.GetString() << '\n';
    }

    etcd::Index get_modified_index() const {
        if ((! document_.HasMember(kNode)) ||
            (!document_[kNode].HasMember(kModifiedIndex))) {
            throw std::runtime_error("possibly timed out");
        }
        return document_[kNode][kModifiedIndex].GetUint64();
    }

  private:
    // CONSTANTS
    const char *kErrorCode ="errorCode";
    const char *kMessage = "message";
    const char *kCause ="cause";
    const char *kNode = "node";
    const char *kModifiedIndex = "modifiedIndex";

    rapidjson::Document document_;
    std::string header_;

    // OPERATIONS
    void _CheckError() { 
        if (document_.HasMember(kErrorCode)) {
            throw etcd::ReplyException(document_[kErrorCode].GetInt(),
                    document_[kMessage].GetString(),
                    document_[kCause].GetString());
        }
    }

    void _Parse(const std::string& json) {
        document_.Parse(json.c_str());
        _CheckError();
    }
};

} // namespace example

#endif // __RAPID_REPLY_HPP_INCLUDED__
