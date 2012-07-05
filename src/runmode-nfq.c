/* Copyright (C) 2007-2011 Open Information Security Foundation
 *
 * You can copy, redistribute or modify this Program under the terms of
 * the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/**
 * \file
 *
 * \author Anoop Saldanha <anoopsaldanha@gmail.com>
 * \author Eric Leblond <eric@regit.org>
 *
 * Handling of NFQ runmodes.
 */


#include "suricata-common.h"
#include "tm-threads.h"
#include "conf.h"
#include "runmodes.h"
#include "runmode-nfq.h"
#include "log-httplog.h"
#include "output.h"
#include "cuda-packet-batcher.h"
#include "source-pfring.h"

#include "alert-fastlog.h"
#include "alert-prelude.h"
#include "alert-unified2-alert.h"
#include "alert-debuglog.h"

#include "util-debug.h"
#include "util-time.h"
#include "util-cpu.h"
#include "util-affinity.h"
#include "util-runmodes.h"

static const char *default_mode;

const char *RunModeIpsNFQGetDefaultMode(void)
{
    return default_mode;
}

void RunModeIpsNFQRegister(void)
{
    default_mode = "autofp";
    RunModeRegisterNewRunMode(RUNMODE_NFQ, "auto",
                              "Multi threaded NFQ IPS mode",
                              RunModeIpsNFQAuto);

    RunModeRegisterNewRunMode(RUNMODE_NFQ, "autofp",
                              "Multi threaded NFQ IPS mode with respect to flow",
                              RunModeIpsNFQAutoFp);

    RunModeRegisterNewRunMode(RUNMODE_NFQ, "workers",
                              "Multi queue NFQ IPS mode with one thread per queue",
                              RunModeIpsNFQWorker);
    return;
}

/**
 * \brief RunModeIpsNFQAuto set up the following thread packet handlers:
 *        - Receive thread (from NFQ)
 *        - Decode thread
 *        - Stream thread
 *        - Detect: If we have only 1 cpu, it will setup one Detect thread
 *                  If we have more than one, it will setup num_cpus - 1
 *                  starting from the second cpu available.
 *        - Veredict thread (NFQ)
 *        - Respond/Reject thread
 *        - Outputs thread
 *        By default the threads will use the first cpu available
 *        except the Detection threads if we have more than one cpu.
 *
 * \param de_ctx Pointer to the Detection Engine.
 *
 * \retval 0 If all goes well. (If any problem is detected the engine will
 *           exit()).
 */
int RunModeIpsNFQAuto(DetectEngineCtx *de_ctx)
{
    SCEnter();
    int ret = 0;
#ifdef NFQ

    RunModeInitialize();

    TimeModeSetLive();

    ret = RunModeSetIPSAuto(de_ctx,
            NFQGetThread,
            "ReceiveNFQ",
            "VerdictNFQ",
            "DecodeNFQ");
#endif /* NFQ */
    return ret;
}


int RunModeIpsNFQAutoFp(DetectEngineCtx *de_ctx)
{
    SCEnter();
    int ret = 0;
#ifdef NFQ

    RunModeInitialize();

    TimeModeSetLive();

    ret = RunModeSetIPSAutoFp(de_ctx,
            NFQGetThread,
            "ReceiveNFQ",
            "VerdictNFQ",
            "DecodeNFQ");
#endif /* NFQ */
    return ret;
}

int RunModeIpsNFQWorker(DetectEngineCtx *de_ctx)
{
    SCEnter();
    int ret = 0;
#ifdef NFQ

    RunModeInitialize();

    TimeModeSetLive();

    ret = RunModeSetIPSWorker(de_ctx,
            NFQGetThread,
            "ReceiveNFQ",
            "VerdictNFQ",
            "DecodeNFQ");
#endif /* NFQ */
    return ret;
}
