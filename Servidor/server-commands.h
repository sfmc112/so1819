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
int cmdLoad(char* token);
int cmdSave(char* token);
int cmdFree(char* token);
int cmdStats(int* print);
int cmdUsers();
int cmdText();
int checkCommandArgs(char* token);

void freeLine(int lineNumber);
void printEditor();
void* editorStats(void* param);
void printUsers();
void loadDocument(char* nomeFicheiro);
void saveDocument(char* nomeFicheiro);
void freeOneLine(int lineNumber);
#endif /* COMMANDS_H */
