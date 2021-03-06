/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#define NOWALLETLIB
#include "tokenlib.h"
#include "options.h"

void reportByToken(const COptions& options);
void reportByAccount(const COptions& options);
//--------------------------------------------------------------
int main(int argc, const char *argv[]) {
    // Tell the system where the blocks are and which version to use
    etherlib_init("parity");

    // Parse command line, allowing for command files
    COptions options;
    if (!options.prepareArguments(argc, argv))
        return 0;

    while (!options.commandList.empty()) {

        SFString command = nextTokenClear(options.commandList, '\n');
        if (!options.parseArguments(command))
            return 0;

        if (options.asData)
            cout << "block\ttoken\tholder\ttoken balance\n";
        if (options.byAccount)
            reportByAccount(options);
        else
            reportByToken(options);

    }
    return 0;
}

//--------------------------------------------------------------
void reportByToken(const COptions& options) {

    bool needsNewline = true;
    // For each token contract
    SFString tokens = options.tokens;
    while (!tokens.empty()) {
        SFAddress token = nextTokenClear(tokens, '|');
        if (!options.asData)
            cout << "\n  For token contract: " << bBlue << token << cOff << "\n";

        // For each holder
        SFString holders = options.holders;
        while (!holders.empty()) {
            SFAddress holder = nextTokenClear(holders, '|');

            // For each block
            SFString blocks = options.blocks.toString();
            while (!blocks.empty()) {

                blknum_t blockNum = toLongU(nextTokenClear(blocks, '|'));
                SFUintBN bal = getTokenBalance(token, holder, blockNum);
                SFString sBal = to_string(bal).c_str();
                if (expContext().asEther) {
                    sBal = wei2Ether(to_string(bal).c_str());
                } else if (expContext().asDollars) {
                    CBlock blk;
                    getBlock(blk, blockNum);
                    sBal = padLeft(dispDollars(blk.timestamp, bal),14);
                }

                needsNewline = true;
                if (bal > 0 || !options.noZero) {
                    if (options.asData) {
                        cout << blockNum << "\t" << token << "\t" << holder << "\t" << sBal << "\n";
                    } else {
                        cout << "    Balance for account " << cGreen << holder << cOff;
                        cout << " at block " << cTeal << blockNum << cOff;
                        cout << " is " << cYellow << sBal << cOff << "\n";
                    }
                    needsNewline = false;
                } else if (!isTestMode()) {
                    if (options.asData) {
                        cerr << blockNum << "\t" << token << "\t" << holder << "         \r";
                    } else {
                        cerr << "    Balance for account " << cGreen << holder << cOff;
                        cerr << " at block " << cTeal << blockNum << cOff << "           \r";
                    }
                }
                cerr.flush();
                cout.flush();
            }
        }
    }
    if (needsNewline)
        cerr << "                                                                                              \n";
}

//--------------------------------------------------------------
void reportByAccount(const COptions& options) {

    bool needsNewline = true;
    // For each holder
    SFString holders = options.holders;
    while (!holders.empty()) {
        SFAddress holder = nextTokenClear(holders, '|');
        if (!options.asData)
            cout << "\n  For account: " << bBlue << holder << cOff << "\n";

        // For each token contract
        SFString tokens = options.tokens;
        while (!tokens.empty()) {
            SFAddress token = nextTokenClear(tokens, '|');

            // For each block
            SFString blocks = options.blocks.toString();
            while (!blocks.empty()) {

                blknum_t blockNum = toLongU(nextTokenClear(blocks, '|'));
                SFUintBN bal = getTokenBalance(token, holder, blockNum);
                SFString sBal = to_string(bal).c_str();
                if (expContext().asEther) {
                    sBal = wei2Ether(to_string(bal).c_str());
                } else if (expContext().asDollars) {
                    CBlock blk;
                    getBlock(blk, blockNum);
                    sBal = padLeft(dispDollars(blk.timestamp, bal),14);
                }

                needsNewline = true;
                if (bal > 0 || !options.noZero) {
                    if (options.asData) {
                        cout << blockNum << "\t" << token << "\t" << holder << "\t" << sBal << "\n";
                    } else {
                        cout << "    Balance of token contract " << cGreen << token << cOff;
                        cout << " at block " << cTeal << blockNum << cOff;
                        cout << " is " << cYellow << sBal << cOff << "\n";
                    }
                    needsNewline = false;
                } else if (!isTestMode()) {
                    if (options.asData) {
                        cout << blockNum << "\t" << token << "\t" << holder << "\r";
                    } else {
                        cout << "    Balance of token contract " << cGreen << token << cOff;
                        cout << " at block " << cTeal << blockNum << cOff << "\r";
                    }
                }
                cerr.flush();
                cout.flush();
            }
        }
    }
    if (needsNewline)
        cerr << "                                                                                              \n";
}
