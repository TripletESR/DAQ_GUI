#include "wfg_ui.h"
#include "ui_wfg_ui.h"

WFG_UI::WFG_UI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WFG_UI)
{
    ui->setupUi(this);
}

WFG_UI::~WFG_UI()
{
    delete ui;
}
