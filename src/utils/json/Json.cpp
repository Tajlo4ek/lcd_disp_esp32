#include "Json.h"

Json::Json(const String &json)
{
    this->data = json;
}

Json::Json(const std::vector<String> &names, const std::vector<String> &datas)
{
    if (names.size() != datas.size())
    {
        this->data = "";
    }

    auto size = names.size();
    this->data = '{';
    for (int dataInd = 0; dataInd < size; dataInd++)
    {
        this->data += '"';
        this->data += names[dataInd];
        this->data += '"';
        this->data += ':';

        auto startChar = datas[dataInd][0];
        if (startChar != '{' && startChar != '[')
        {
            this->data += '"';
        }

        this->data += datas[dataInd];

        if (startChar != '{' && startChar != '[')
        {
            this->data += '"';
        }

        if (dataInd + 1 != size)
        {
            this->data += ',';
        }
    }

    this->data += '}';
}

const Json Json::SubJson(int startInd, int &finishInd) const
{
    int openBracket = 0;
    int countQuote = 0;
    for (uint i = startInd; i < this->data.length(); i++)
    {
        auto ch = this->data[i];
        if (ch == '[' || ch == '{')
        {
            openBracket++;
        }
        else if (ch == ']' || ch == '}')
        {
            openBracket--;
        }
        else if (ch == '"')
        {
            countQuote++;
        }

        if ((ch == ',' && openBracket == 0) || openBracket < 0)
        {
            finishInd = i;
            break;
        }
    }


    if (startInd > finishInd)
    {
        return Json("");
    }

    String res = this->data.substring(startInd, finishInd);
    res.trim();

    if (countQuote == 2 && res[0] == '"' && res[res.length() - 1] == '"')
    {
        res = res.substring(1, res.length() - 1);
    }

    return Json(res);
}

const bool Json::ContainsName(String name) const
{
    if (name[0] != '"')
    {
        name = '"' + name + '"';
    }

    if (name[name.length() - 1] != ':')
    {
        name = name + ":";
    }

    return this->data.indexOf(name) != -1;
}

const Json Json::operator[](String name) const
{
    if (name[0] != '"')
    {
        name = '"' + name + '"';
    }

    if (name[name.length() - 1] != ':')
    {
        name += ':';
    }
    auto dataStart = this->data.indexOf(name);


    if (dataStart == -1)
    {
        return Json("");
    }

    int startInd = dataStart + name.length();
    int _del;

    return SubJson(startInd, _del);
}

const std::vector<Json> Json::GetArray() const
{
    std::vector<Json> array;

    int startInd = 1;
    int finishInd = 2;

    while (finishInd < data.length())
    {
        Json json = SubJson(startInd, finishInd);

        if (json.data.isEmpty())
        {
            break;
        }

        array.push_back(json);
        startInd = finishInd + 1;
    }

    return array;
}

const Json Json::operator[](uint index) const
{
    if (data[0] != '[' || data[data.length() - 1] != ']')
    {
        return Json("");
    }

    std::vector<Json> array = GetArray();

    return index < array.size() ? array[index] : Json("");
}

const String Json::ToString() const
{
    return data;
}

const int Json::ToInt() const
{
    return data.toInt();
}

const float Json::ToFloat() const
{
    return data.toFloat();
}