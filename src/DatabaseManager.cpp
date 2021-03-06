/**
 * @file DatabaseManager.cpp
 * @author Shubham Kamble (ufrshubham@gmail.com)
 * @brief Method definitions for DatabaseManager class.
 * @version 0.1
 * @date 2020-05-16
 *
 * Copyright (c) 2020 Shubham Kamble
 *
 */

#include "DatabaseManager.hpp"
#include <stdexcept>
#include <cassert>
#include <sstream>
#include <iostream>

DatabaseManager::DatabaseManager() : m_dbRef(nullptr) {}

DatabaseManager::~DatabaseManager()
{
    sqlite3_close(m_dbRef);
}

void DatabaseManager::OpenDatabase(const std::string &databaseName)
{
    sqlite3_open(databaseName.c_str(), &m_dbRef);

    if (!m_dbRef)
    {
        throw std::runtime_error("Failed to open " + databaseName);
    }
}

void DatabaseManager::CreateTable()
{
    if (!m_dbRef)
    {
        throw std::runtime_error("Trying to create table without creating or opening a database.");
    }

    constexpr const char *createTableSql = "CREATE TABLE IF NOT EXISTS TODO("
                                           "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                           "TITLE TEXT NOT NULL, "
                                           "STATUS INTEGER NOT NULL);";

    char *errorMsg = nullptr;
    if (sqlite3_exec(m_dbRef, createTableSql, nullptr, nullptr, &errorMsg) != SQLITE_OK)
    {
        // Create a string object so that errorMsg can be freed before we throw.
        std::string errorStr(errorMsg);
        sqlite3_free(errorMsg);
        throw std::runtime_error(errorStr);
    }
}

void DatabaseManager::AddTask(const Task &task)
{
    if (!m_dbRef)
    {
        throw std::runtime_error("Trying to add a task before creating or opening a database.");
    }

    std::stringstream ss;
    ss << "INSERT INTO TODO (TITLE, STATUS) VALUES('" << task.GetTitle() << "', " << (int)task.GetStatus() << ");";

    char *errorMsg = nullptr;
    if (sqlite3_exec(m_dbRef, ss.str().c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK)
    {
        // Create a string object so that errorMsg can be freed before we throw.
        std::string errorStr(errorMsg);
        sqlite3_free(errorMsg);
        throw std::runtime_error(errorStr);
    }
}

std::vector<Task> DatabaseManager::GetAllTasks() const
{
    std::vector<Task> tasks;

    if (m_dbRef)
    {
        constexpr const char *selectAllSql = "SELECT * FROM TODO";
        char *errorMsg = nullptr;

        if (sqlite3_exec(m_dbRef, selectAllSql, DatabaseManager::CollectTasksCallBack, reinterpret_cast<void *>(&tasks), &errorMsg) != SQLITE_OK)
        {
            // Create a string object so that errorMsg can be freed before we throw.
            std::string errorStr(errorMsg);
            sqlite3_free(errorMsg);
            throw std::runtime_error(errorStr);
        }
    }
    return tasks;
}

void DatabaseManager::RemoveTask(int taskID)
{
    if (!m_dbRef)
    {
        throw std::runtime_error("Trying to delete a task before creating or opening a database.");
    }

    std::stringstream ss;
    ss << "DELETE FROM TODO WHERE ID = '" << taskID << "'";

    char *errorMsg = nullptr;
    if (sqlite3_exec(m_dbRef, ss.str().c_str(), nullptr, nullptr, &errorMsg) != SQLITE_OK)
    {
        // Create a string object so that errorMsg can be freed before we throw.
        std::string errorStr(errorMsg);
        sqlite3_free(errorMsg);
        throw std::runtime_error(errorStr);
    }
}

int DatabaseManager::GetNextTaskID() const
{
    int nextTaskID = 0;

    // This lambda will be called when sqlite engine runs our query stored in ss below.
    auto collectNextTaskID = [](void *ptr, int nEntries, char **entries, char **columns) -> int {
        if (ptr)
        {
            // We can safely cast it to int*
            int *nextTaskID = reinterpret_cast<int *>(ptr);
            if (nextTaskID)
            {
                // I could have directly used index 1 to get the ID from the entries we get.
                // But it does not hurt to have a few checks.
                for (int i = 0; i < nEntries; ++i)
                {
                    if (columns[i] == std::string("seq"))
                    {
                        *nextTaskID = std::stoi(entries[i]) + 1;
                    }
                }
            }
        }
        return 0;
    };

    std::stringstream ss;
    ss << "SELECT * FROM SQLITE_SEQUENCE WHERE NAME='TODO'";

    char *errorMsg = nullptr;
    if (sqlite3_exec(m_dbRef, ss.str().c_str(), collectNextTaskID, &nextTaskID, &errorMsg) != SQLITE_OK)
    {
        // Create a string object so that errorMsg can be freed before we throw.
        std::string errorStr(errorMsg);
        sqlite3_free(errorMsg);
        throw std::runtime_error(errorStr);
    }

    return nextTaskID;
}

int DatabaseManager::CollectTasksCallBack(void *ptrToTaskVec, int nEntries, char **entries, char **columns)
{
    if (ptrToTaskVec)
    {
        std::vector<int> idVec;
        std::vector<std::string> titleVec;
        std::vector<TaskStatus> statusVec;

        for (int i = 0; i < nEntries; ++i)
        {
            if (columns[i] == std::string("ID"))
            {
                idVec.emplace_back(std::stoi(entries[i]));
            }
            else if (columns[i] == std::string("TITLE"))
            {
                titleVec.emplace_back(entries[i]);
            }
            else if (columns[i] == std::string("STATUS"))
            {
                statusVec.emplace_back((TaskStatus)std::stoi(entries[i]));
            }
        }

        if (auto vecTasks = reinterpret_cast<std::vector<Task> *>(ptrToTaskVec))
        {
            for (int i = 0; i < titleVec.size(); ++i)
            {
                std::cout << idVec[i] << std::endl;
                auto task = Task(idVec[i]);
                task.SetTitle(titleVec.at(i));
                task.SetStatus(statusVec.at(i));
                vecTasks->push_back(task);
            }
        }
        else
        {
            // This will make sure that sqlite stops further processing.
            return 1;
        }
    }
    return 0;
}
