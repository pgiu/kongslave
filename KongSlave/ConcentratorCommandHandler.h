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
#ifndef __CONCENTRATOR_COMMAND_HANDLER_H_
#define __CONCENTRATOR_COMMAND_HANDLER_H_

#include <opendnp3/master/ISOEHandler.h>
#include <asiodnp3/IOutstation.h>
#include <asiopal/UTCTimeSource.h>

#include "asiodnp3/OutstationStackImpl.h"
#include <iostream>
#include <sstream>

#include <time.h>

#include <opendnp3/outstation/ICommandHandler.h>

#include "BlockingCommandCallback.h"


using namespace opendnp3;

namespace kongcentrator
{

	/**
	* Mock ICommandHandler used for examples and demos
	*/
	class ConcentratorCommandHandler : public ICommandHandler
	{
	public:

		/**
		* @param aStatusFunc functor used to retrieve that next CommandStatus enumeration
		*/
		ConcentratorCommandHandler(CommandStatus status);

		CommandStatus Select(const ControlRelayOutputBlock& arCommand, uint16_t aIndex) override final;
		CommandStatus Operate(const ControlRelayOutputBlock& arCommand, uint16_t aIndex) override final;


		CommandStatus Select(const AnalogOutputInt16& arCommand, uint16_t aIndex) override final;
		CommandStatus Operate(const AnalogOutputInt16& arCommand, uint16_t aIndex) override final;


		CommandStatus Select(const AnalogOutputInt32& arCommand, uint16_t aIndex) override final;
		CommandStatus Operate(const AnalogOutputInt32& arCommand, uint16_t aIndex) override final;


		CommandStatus Select(const AnalogOutputFloat32& arCommand, uint16_t aIndex) override final;
		CommandStatus Operate(const AnalogOutputFloat32& arCommand, uint16_t aIndex) override final;


		CommandStatus Select(const AnalogOutputDouble64& arCommand, uint16_t aIndex) override final;
		CommandStatus Operate(const AnalogOutputDouble64& arCommand, uint16_t aIndex) override final;

	protected:
		CommandStatus status;
		uint32_t numInvocations;

	};


	class SuccessCommandHandler : public ConcentratorCommandHandler
	{
	public:

		static SuccessCommandHandler& Instance()
		{
			return handler;
		}


	private:

		static SuccessCommandHandler handler;

	protected:
		SuccessCommandHandler();
	};

}

#endif

