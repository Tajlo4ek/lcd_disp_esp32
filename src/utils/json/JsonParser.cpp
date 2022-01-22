#include "JsonParser.h"

namespace JsonParser
{
    String GetJsonData(const String &json, const String &name, bool &isOk)
    {
        String findName = name;
        if (findName[0] != '"')
        {
            findName = '"';
            findName += name;
            findName += '"';
        }

        auto dataStart = json.indexOf(findName);
        if (dataStart == -1)
        {
            isOk = false;
            return String();
        }

        int startInd = json.indexOf(':', dataStart);
        if (startInd == -1)
        {
            isOk = false;
            return String();
        }

        String res;
        int openBracket = 0;
        int countQuote = 0;
        for (uint i = startInd + 1; i < json.length(); i++)
        {
            auto ch = json[i];
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
                break;
            }

            res += ch;
        }

        res.trim();
        if (countQuote == 2)
        {
            res.replace(String('"'), String());
        }

        isOk = true;
        return res;
    }

    String GetJsonData(const String &json, const String &name)
    {
        bool isOk = false;
        return GetJsonData(json, name, isOk);
    }

    void ParseJsonArray(const String &json, const int itemCount, String *output)
    {
        int index = 1;
        int countBracket = 0;
        int parsed = 0;
        String buf;

        while (true)
        {
            char ch = json[index];

            if (ch == '{')
            {
                countBracket++;
            }
            else if (ch == '}')
            {
                countBracket--;
            }

            if (countBracket == 0 && (ch == ',' || ch == ']'))
            {
                output[parsed] = buf;
                buf.clear();
                parsed++;

                index = json.indexOf(',', index);

                if (parsed == itemCount)
                {
                    break;
                }
            }
            else
            {
                buf += ch;
            }

            index++;
        }
    }

    String BuildJson(const String *names, const String *data, int dataCount)
    {
        String json = String('{');

        for (int dataInd = 0; dataInd < dataCount; dataInd++)
        {
            json += '"';
            json += names[dataInd];
            json += '"';

            json += ':';

            auto startChar = data[dataInd][0];
            if (startChar != '{' && startChar != '[')
            {
                json += '"';
            }

            json += data[dataInd];

            if (startChar != '{' && startChar != '[')
            {
                json += '"';
            }

            if (dataInd + 1 != dataCount)
            {
                json += ',';
            }
        }

        json += '}';

        return json;
    }

}