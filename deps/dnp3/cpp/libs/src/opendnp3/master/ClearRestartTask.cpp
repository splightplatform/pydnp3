/*
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
#include "ClearRestartTask.h"

#include "opendnp3/app/APDUBuilders.h"
#include "opendnp3/LogLevels.h"
#include "opendnp3/master/MasterTasks.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

ClearRestartTask::ClearRestartTask(const std::shared_ptr<TaskContext>& context, IMasterApplication& application, openpal::Logger logger) :
	IMasterTask(context, application, TaskBehavior::ReactsToIINOnly(), logger, TaskConfig::Default())
{

}

bool ClearRestartTask::BuildRequest(APDURequest& request, uint8_t seq)
{
	build::ClearRestartIIN(request, seq);
	return true;
}

IMasterTask::ResponseResult ClearRestartTask::ProcessResponse(const APDUResponseHeader& response, const openpal::RSlice& objects)
{
	// we only care that the response to this has FIR/FIN
	if (ValidateSingleResponse(response))
	{
		if (response.IIN.IsSet(IINBit::DEVICE_RESTART))
		{
			// we tried to clear the restart, but the device responded with the restart still set
			SIMPLE_LOG_BLOCK(logger, flags::ERR, "Clear restart task failed to clear restart bit, permanently disabling task");
			return ResponseResult::ERROR_BAD_RESPONSE;
		}
		else
		{
			return  ResponseResult::OK_FINAL;
		}
	}
	else
	{
		return  ResponseResult::ERROR_BAD_RESPONSE;
	}
}

} //end ns

