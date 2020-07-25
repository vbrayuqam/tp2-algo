cmake_minimum_required(VERSION 3.16)
project(tp2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(tp2
        AttributNonDefinieXML.cpp
        AttributNonDefinieXML.hpp
        DocumentXML.cpp
        DocumentXML.hpp
        ElementXML.cpp
        ElementXML.hpp
        Histoire.cpp
        Histoire.hpp
        Lecteur.cpp
        Lecteur.hpp
        NoeudXML.cpp
        NoeudXML.hpp
        Phrase.cpp
        Phrase.hpp
        TexteXML.cpp
        TexteXML.hpp
        TP2.cpp)
