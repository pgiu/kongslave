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
#ifndef __BLOCKING_COMMAND_CALLBACK_H_
#define __BLOCKING_COMMAND_CALLBACK_H_

#include <openpal/util/Uncopyable.h>
#include <opendnp3/master/CommandResponse.h>
#include <opendnp3/master/ITaskCallback.h>


#include <asiopal/Synchronized.h>

namespace opendnp3
{

/**
* Callback when a command finishes or fails
*/
class BlockingCommandCallback : public ICommandCallback, private openpal::Uncopyable
{	
	
public:	
	
	virtual void OnComplete(const opendnp3::CommandResponse& response) override final;

	opendnp3::CommandResponse WaitForResult();

private:

	asiopal::Synchronized<opendnp3::CommandResponse> response;
};

}

#endif
