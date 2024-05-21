#include "PBResponse.h"

#include <iostream>
#include <sstream>
#include <boost/locale.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "..//Common/CFunctions.hpp"
#include "..//Common/MessageDlg.h"



PBResponse::PBResponse()
{
    succeed = false;
}


PBResponse::PBResponse(std::string res)
{

    boost::property_tree::ptree pt;
    try
    {

        std::stringstream ss;
        ss << boost::locale::conv::utf_to_utf<char>(res);

        boost::property_tree::read_json(ss, pt);
        succeed = pt.get_child("succeed").get_value<std::string>() == "true";
        error = UTF8ToWstring(pt.get_child("errorMsg").get_value<std::string>());

        boost::property_tree::ptree paramsNode = pt.get_child("params");



        for (boost::property_tree::ptree::iterator it = paramsNode.begin(); it != paramsNode.end(); ++it)
        {
            std::map<std::string, std::wstring> params;


            if (it->second.begin() != it->second.end() && it->second.begin()->first == "")
            {
                boost::property_tree::ptree secTree = it->second;

                for (boost::property_tree::ptree::iterator itSection = secTree.begin(); itSection != secTree.end(); ++itSection)
                {

                    for (boost::property_tree::ptree::iterator itP = itSection->second.begin(); itP != itSection->second.end(); ++itP)
                    {

                        std::string key = itP->first;
                        std::wstring value = UTF8ToWstring(itP->second.get_value<std::string>());
                        params[key] = value;
                    }

                    resultVec[it->first].push_back(params);
                }
            }
            else
            {
                for (boost::property_tree::ptree::iterator itSection = it->second.begin(); itSection != it->second.end(); ++itSection)
                {

                    std::string key = itSection->first;
                    std::wstring value = UTF8ToWstring(itSection->second.get_value<std::string>());
                    params[key] = value;

                }
                resultMap[it->first] = params;
            }

           


        }


    }
    catch (const std::exception& err)
    {
        succeed = false;
        error = L"Incorrect server response";
        MessageDlg(NULL, L"Server Error",
            UTF8ToWstring(std::string(err.what())).c_str(),
            MD_OK, MD_ERR).OpenDlg();
    }


    int a = 0;
}

bool PBResponse::Succeed()
{
	return succeed;
}
const std::wstring& PBResponse::GetError()
{
	return error;
}
std::vector<std::map<std::string, std::wstring>> PBResponse::GetResult()
{
    return resultVec["main"];
}

std::map<std::string, std::wstring> PBResponse::GetResultMap(std::string key)
{
    return resultMap[key];
}
std::vector<std::map<std::string, std::wstring>> PBResponse::GetResultVec(std::string key)
{
    return resultVec[key];
}