/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "basetypes.h"
#include "options_base.h"
#include "conversions.h"

namespace qblocks {

    //--------------------------------------------------------------------------------
    blknum_t COptionsBlockList::parseBlockOption(SFString& msg, blknum_t lastBlock) const {

        blknum_t ret = NOPOS;

        SFString arg = msg;
        msg = "";

        // if it's a number, return it
        if (isNumeral(arg) || isHexStr(arg)) {
            ret = toUnsigned(arg);

        } else {
            // if it's not a number, it better be a special value, and we better be able to find it
            CNameValue spec;
            if (pOptions && pOptions->findSpecial(spec, arg)) {
                if (spec.getName() == "latest") {
                    ret = lastBlock;
                } else {
                    ret = toUnsigned(spec.getValue());
                }
            } else {
                msg = "The given value '" + arg + "' is not a numeral or a special named block. Quitting...";
                return NOPOS;
            }
        }

        if (ret > lastBlock) {
            SFString lateStr = (isTestMode() ? "--" : asStringU(lastBlock));
            msg = "Block " + arg + " is later than the last valid block " + lateStr + ". Quitting...\n";
            return NOPOS;
        }

        return ret;
    }

    //--------------------------------------------------------------------------------
    SFString COptionsBlockList::parseBlockList(const SFString& argIn, blknum_t lastBlock) {
        SFString arg = argIn;
        if (arg.Contains("-")) {

            // If we already have a range, bail
            if (start != stop)
                return "Specify only a single block range at a time.";

            SFString stp = arg;
            SFString strt = nextTokenClear(stp, '-');
            if (strt == "latest")
                return "Cannot start range with 'latest'";

            SFString msg = strt;
            start = parseBlockOption(msg, lastBlock);
            if (!msg.empty())
                return msg;
            msg = stp;
            stop = parseBlockOption(msg, lastBlock);
            if (!msg.empty())
                return msg;

            if (stop <= start)
                return "'stop' must be strictly larger than 'start'";

            blknum_t width = (stop - start + 1);
            if (width > MAX_BLOCK_RANGE)
                return "The range you specified (" + argIn + ") is too broad (" + asStringU(width) + "). "
                        "Ranges may be at most " + asStringU(MAX_BLOCK_RANGE) + " blocks long. Quitting...";

        } else {

            if (nNums >= MAX_BLOCK_LIST)
                return "Too many blocks in list. Max is " + asString(MAX_BLOCK_LIST);

            SFString msg = arg;
            blknum_t num = parseBlockOption(msg, lastBlock);
            if (!msg.empty())
                return msg;

            nums[nNums++] = num;

        }

        latest = lastBlock;
        return "";
    }

    //--------------------------------------------------------------------------------
    void COptionsBlockList::Init(void) {
        nums[0]    = NOPOS;
        nNums      = 0;  // we will set this to '1' later if user supplies no values
        start = stop = 0;
    }

    //--------------------------------------------------------------------------------
    COptionsBlockList::COptionsBlockList(void) {
        Init();
    }

    //--------------------------------------------------------------------------------
    SFString COptionsBlockList::toString(void) const {
        SFString ret;
        for (SFUint32 i = start ; i < stop ; i++)
            ret += (asStringU(i) + "|");
        for (SFUint32 i = 0 ; i < nNums ; i++)
            ret += (asStringU(nums[i]) + "|");
        return ret;
    }

}  // namespace qblocks
