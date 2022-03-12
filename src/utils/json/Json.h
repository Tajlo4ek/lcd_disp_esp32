#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include "Arduino.h"

class Json
{
private:
    String data;

    const Json SubJson(int startInd, int &finishInd) const;

public:
    explicit Json(const String &json);
    Json(const std::vector<String> &names, const std::vector<String> &datas);

    const bool ContainsName(String name) const;

    const std::vector<Json> GetArray() const;

    const Json operator[](String name) const;
    const Json operator[](uint index) const;

    const String ToString() const;
    const int ToInt() const;
    const float ToFloat() const;
};

#endif