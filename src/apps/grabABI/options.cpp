/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "options.h"

//---------------------------------------------------------------------------------------------------
CParams params[] = {
    CParams("~addr",      "the address(es) of the smart contract(s) to grab"),
    CParams("-canonical", "convert all types to their canonical represenation and remove all spaces from display"),
    CParams("-generate",  "generate C++ code into the current folder for all functions and events found in the ABI"),
    CParams("-encode",    "generate the encodings for the functions / events in the ABI"),
    CParams("-noconst",   "generate encodings for non-constant functions and events only (always true when generating)"), // NOLINT
    CParams("-open",      "open the ABI file for editing, download if not already present"),
    CParams("-raw",       "force retrieval of ABI from etherscan (ignoring cache)"),
    CParams("@-json",     "print the ABI to the screen as json"),
    CParams("@-silent",   "If ABI cannot be acquired, fail silently (useful for scripting)"),
    CParams("",           "Fetches the ABI for a smart contract. Optionally generates C++ source code "
                          "representing that ABI.\n"),
};
uint32_t nParams = sizeof(params) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
bool COptions::parseArguments(SFString& command) {

    if (!standardOptions(command))
        return false;

    Init();
    while (!command.empty()) {
        SFString arg = nextTokenClear(command, ' ');
        if (arg == "-g" || arg == "--gen" || arg == "--generate") {
            classDir = getCWD();
            prefix = getPrefix(classDir);

        } else if (arg == "-c" || arg == "--canonical") {
            if (parts&SIG_ENCODE)
                parts = (SIG_CANONICAL|SIG_ENCODE);
            else
                parts = SIG_CANONICAL;

        } else if (arg == "-e" || arg == "--encode") {
            parts |= SIG_ENCODE;

        } else if (arg == "-s" || arg == "--silent") {
            silent = true;

        } else if (arg == "-n" || arg == "--noconst") {
            noconst = true;

        } else if (arg == "-r" || arg == "--raw") {
            raw = true;

        } else if (arg == "-o" || arg == "--open") {
            open = true;

        } else if (arg == "-j" || arg == "--json") {
            asJson = true;

        } else if (arg.startsWith('-')) {  // do not collapse

            if (!builtInCmd(arg)) {
                return usage("Invalid option: " + arg);
            }

        } else {
            if (nAddrs>=MAX_ADDRS)
                return usage("You may provide at most " + asString(MAX_ADDRS) + " addresses");
            if (primaryAddr.empty())
                primaryAddr = arg;
            SFAddress addr = fixAddress(toLower(arg));
            if (!isAddress(addr) && !addr.ContainsI("tokenlib") && !addr.ContainsI("walletlib"))
                return usage("Invalid address `" + addr + "'. Length is not equal to 40 characters (20 bytes).\n");
            addrs[nAddrs++] = addr;
        }
    }

    if (parts == 0)
        parts = SIG_DEFAULT;

    if (parts != SIG_CANONICAL && verbose)
        parts |= SIG_DETAILS;

    if (!nAddrs)
        return usage("Please supply at least one Ethereum address.\n");

    return true;
}

//---------------------------------------------------------------------------------------------------
void COptions::Init(void) {
    paramsPtr = params;
    nParamsRef = nParams;
    pOptions = this;

    parts = SIG_DEFAULT;
    noconst = false;
    open = false;
    silent = false;
    asJson = false;
    raw = false;
    for (uint32_t i = 0 ; i < MAX_ADDRS ; i++) {
        addrs[i] = "";
    }
    nAddrs = 0;
}

//---------------------------------------------------------------------------------------------------
COptions::COptions(void) {
    Init();
}

//--------------------------------------------------------------------------------
COptions::~COptions(void) {
}

//--------------------------------------------------------------------------------
SFString COptions::postProcess(const SFString& which, const SFString& str) const {
    if (which == "options") {
        return str;

    } else if (which == "notes" && (verbose || COptions::isReadme)) {
        SFString ret;
        ret += "Use the [{--silent}] option, which displays fewer messages, for scripting.\n";
        return ret;
    }
    return str;
}

//--------------------------------------------------------------------------------
SFString getPrefix(const SFString& inIn) {

    SFString in = inIn; // for example ./ENS/parselib/
    in.Replace("parseLib","parselib"); // hack: to fix dao monitor
    in.Reverse();
    in.Replace("/", ""); // remove trailing '/'
    in = nextTokenClear(in, '/'); // remove /parselib
    in.Reverse();
    return in;
}
