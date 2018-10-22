/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   utils.h
 * Author: ricardo
 *
 * Created on October 11, 2018, 12:03 PM
 */

#ifndef UTILS_H
#define UTILS_H
void exitError(char* mensagem);
void exitNormal();
void toLower(char* buffer);
void toUpper(char* buffer);
int ifFileExists(char* nomeFicheiro);
//char* getSignalName(int signal, char* str);
#endif /* UTILS_H */
