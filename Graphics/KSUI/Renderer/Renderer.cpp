//
// Created by kalasoft on 12/23/21.
//

#include <KSUI/View.h>
#include "Renderer.h"
void Renderer::setDisplayMetrics(const DisplayMetrics displayMetrics) {

    View::dispMetrics = displayMetrics;
    //KSLOGD("Renderer", "DisplayMetrics set");
    displayMetrics.print();
}