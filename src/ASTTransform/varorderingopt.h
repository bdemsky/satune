/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VarOrderingOpt.h
 * Author: hamed
 *
 * Created on October 11, 2018, 5:31 PM
 */

#ifndef VARORDERINGOPT_H
#define VARORDERINGOPT_H

#include "transform.h"


class VarOrderingOpt :Transform {
public:
        VarOrderingOpt(CSolver *_solver, SATEncoder *_satencoder);
        void doTransform();
        virtual ~VarOrderingOpt();
private:
        SATEncoder* satencoder;
};

#endif /* VARORDERINGOPT_H */

