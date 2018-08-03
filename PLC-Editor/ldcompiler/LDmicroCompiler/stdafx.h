// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#include <stddef.h>
#include <stdio.h>


using namespace System;

bool IsNumber(String^ str);
String^ BoolToString(bool value, String^ ifTrue, String^ ifFalse);
String^ RemoveDollar(String^ str);

int IndexOfClosingParenthesis(String^ str);

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
