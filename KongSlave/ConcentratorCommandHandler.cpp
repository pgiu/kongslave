/**
* Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
* more contributor license agreements. See the NOTICE file distributed
* with this work for additional information regarding copyright ownership.
* Green Energy Corp licenses this file to you under the Apache License,
* Version 2.0 (the "License"); you may not use this file except in
* compliance with the License.  You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* This project was forked on 01/01/2013 by Automatak, LLC and modifications
* may have been made to this file. Automatak, LLC licenses these modifications
* to you under the terms of the License.
*/
#include "ConcentratorCommandHandler.h"
#include "opendnp3/gen/CommandStatus.h"
#include "opendnp3/app/ControlRelayOutputBlock.h"
#include <iostream>

using namespace std;
using namespace opendnp3;

using namespace kongcentrator;

ConcentratorCommandHandler::ConcentratorCommandHandler(CommandStatus status_) : status(status_), numInvocations(0)
{

}

CommandStatus ConcentratorCommandHandler::Select(const ControlRelayOutputBlock& arCommand, uint16_t aIndex)
{
	++numInvocations;
	return status;
}
CommandStatus ConcentratorCommandHandler::Operate(const ControlRelayOutputBlock& arCommand, uint16_t aIndex)
{
	cout << "[ControlRelay] index: " << aIndex <<
		", Control Code: "		<< ControlCodeToString(arCommand.functionCode) <<
		", Command Status: "	<< CommandStatusToString(arCommand.status) << 
		endl;
	++numInvocations;
	return status;
}

CommandStatus ConcentratorCommandHandler::Select(const AnalogOutputInt16& arCommand, uint16_t aIndex)
{
	++numInvocations;
	return status;
}
CommandStatus ConcentratorCommandHandler::Operate(const AnalogOutputInt16& arCommand, uint16_t aIndex)
{
	cout << "[AnalogOutput16] index: " << aIndex <<
		", Value: " << arCommand.value <<
		", Command Status: " << CommandStatusToString(arCommand.status) <<
		endl;
	++numInvocations;
	return status;
}

CommandStatus ConcentratorCommandHandler::Select(const AnalogOutputInt32& arCommand, uint16_t aIndex)
{
	return status;
}
CommandStatus ConcentratorCommandHandler::Operate(const AnalogOutputInt32& arCommand, uint16_t aIndex)
{
	cout << "[AnalogOutputFloat16] index: " << aIndex <<
		", Value: " << arCommand.value <<
		", Command Status: " << CommandStatusToString(arCommand.status) <<
		endl;
	++numInvocations;
	return status;
}

CommandStatus ConcentratorCommandHandler::Select(const AnalogOutputFloat32& arCommand, uint16_t aIndex)
{
	++numInvocations;
	return status;
}
CommandStatus ConcentratorCommandHandler::Operate(const AnalogOutputFloat32& arCommand, uint16_t aIndex)
{
	cout << "[AnalogOutputFloat32] index: " << aIndex <<
		", Value: " << arCommand.value <<
		", Command Status: " << CommandStatusToString(arCommand.status) <<
		endl;
	++numInvocations;
	return status;
}

CommandStatus ConcentratorCommandHandler::Select(const AnalogOutputDouble64& arCommand, uint16_t aIndex)
{
	++numInvocations;
	return status;
}
CommandStatus ConcentratorCommandHandler::Operate(const AnalogOutputDouble64& arCommand, uint16_t aIndex)
{
	cout << "[AnalogOutputDouble64] index: " << aIndex <<
		", Value: " << arCommand.value <<
		", Command Status: " << CommandStatusToString(arCommand.status) <<
		endl;
	++numInvocations;
	return status;
}

SuccessCommandHandler::SuccessCommandHandler() : ConcentratorCommandHandler(CommandStatus::SUCCESS)
{
	std::cout << "Success!" << std::endl;
}

SuccessCommandHandler SuccessCommandHandler::handler;







