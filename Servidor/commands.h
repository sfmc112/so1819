/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   commands.h
 * Author: ricardo
 *
 * Created on October 11, 2018, 12:08 PM
 */

#ifndef COMMANDS_H
#define COMMANDS_H

void cmdShutdown();
void cmdSettings();
int cmdLoad();
int cmdSave();
int cmdFree();
int cmdStats();
int cmdUsers();
int cmdText();
int checkCommandArgs(char* token);

#endif /* COMMANDS_H */
