#pragma once
#ifndef INCLUDED_CMDSAVE_H
#define INCLUDED_CMDSAVE_H

#include "Cmd.h"

class CmdSave :	public Cmd
{
public:
	CmdSave();
	~CmdSave();

	void execute(MainView* view, Persist* persist);
};

#endif