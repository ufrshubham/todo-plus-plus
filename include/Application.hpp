/**
 * @file Application.hpp
 * @author Shubham Kamble (ufrshubham@gmail.com)
 * @brief This file contains the Application class, which can create a window and handle inputs.
 * @version 0.1
 * @date 2020-07-22
 * 
 * Copyright (c) 2020 Shubham Kamble
 * 
 */

#pragma once

#include "DatabaseManager.hpp"

class Application
{
    /**
     * @brief The database manager.
     * 
     */
    DatabaseManager m_dman;

    /**
     * @brief Path to currently opened database. Not sure if this is really needed.
     * 
     */
    std::string m_databasePath;

    /**
     * @brief Initilizes the UI and TGUI widgets.
     * 
     */
    void InitUI();

    /**
     * @brief Opens the database file using m_databasePath. Make sure to check for the returned status.
     *        If any error occues during opening the database, DatabaseStatus::FAILED will be returned.
     * 
     * @return DatabaseStatus 
     */
    DatabaseStatus InitDB();

    /**
     * @brief This method launches a sub-windows to takes user inputs
     *        and then add a new task in UI as well as database.
     *        Right now this is a temporary solution and might have to be reworked.
     * 
     */
    void AddTaskGui();
    
    /**
     * @brief This method removed the selected item from UI view.
     *        [NOTE] : It does not yet remove the actual task from database.
     * 
     */
    void DeleteSelected();

public:

    /**
     * @brief Construct a new Application object
     * 
     * @param width Initial window width for this application.
     * @param height Initial window height for this application.
     * @param title Title for this application.
     * @param databasePath Full path to the database file to be opened. Will be created if not already present.
     */
    Application(unsigned int width, unsigned int height, const std::string &title, const std::string &databasePath);

    /**
     * @brief Destroy the Application object
     * 
     */
    ~Application();

    /**
     * @brief Starts the application by displaying the application window and listening for events.
     * 
     */
    void Run();
};
