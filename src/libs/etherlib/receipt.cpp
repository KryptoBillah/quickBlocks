/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
/*
 * This file was generated with makeClass. Edit only those parts of the code inside
 * of 'EXISTING_CODE' tags.
 */
#include "receipt.h"
#include "etherlib.h"

namespace qblocks {

//---------------------------------------------------------------------------
IMPLEMENT_NODE(CReceipt, CBaseNode);

//---------------------------------------------------------------------------
extern SFString nextReceiptChunk(const SFString& fieldIn, const void *dataPtr);
static SFString nextReceiptChunk_custom(const SFString& fieldIn, const void *dataPtr);

//---------------------------------------------------------------------------
void CReceipt::Format(CExportContext& ctx, const SFString& fmtIn, void *dataPtr) const {
    if (!m_showing)
        return;

    if (fmtIn.empty()) {
        ctx << toJson();
        return;
    }

    SFString fmt = fmtIn;
    if (handleCustomFormat(ctx, fmt, dataPtr))
        return;

    while (!fmt.empty())
        ctx << getNextChunk(fmt, nextReceiptChunk, this);
}

//---------------------------------------------------------------------------
SFString nextReceiptChunk(const SFString& fieldIn, const void *dataPtr) {
    if (dataPtr)
        return ((const CReceipt *)dataPtr)->getValueByName(fieldIn);

    // EXISTING_CODE
    // EXISTING_CODE

    return fldNotFound(fieldIn);
}

//---------------------------------------------------------------------------------------------------
bool CReceipt::setValueByName(const SFString& fieldName, const SFString& fieldValue) {
    // EXISTING_CODE
    if (fieldName == "contractAddress" && fieldValue == "null") {
        *((SFString*)&fieldValue) = "0";
    } else if (fieldName == "status") {
        isError = (fieldValue == "0");
        return true;
    }

    if (pTrans)
        if (((CTransaction*)pTrans)->setValueByName(fieldName, fieldValue))
            return true;
    // EXISTING_CODE

    switch (tolower(fieldName[0])) {
        case 'c':
            if ( fieldName % "contractAddress" ) { contractAddress = toAddress(fieldValue); return true; }
            break;
        case 'g':
            if ( fieldName % "gasUsed" ) { gasUsed = toGas(fieldValue); return true; }
            break;
        case 'l':
            if ( fieldName % "logs" ) {
                char *p = (char *)fieldValue.c_str();
                while (p && *p) {
                    CLogEntry item;
                    uint32_t nFields = 0;
                    p = item.parseJson(p, nFields);
                    if (nFields)
                        logs[logs.getCount()] = item;
                }
                return true;
            }
            if ( fieldName % "logsBloom" ) { logsBloom = toBloom(fieldValue); return true; }
            break;
        default:
            break;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------
void CReceipt::finishParse() {
    // EXISTING_CODE
    // EXISTING_CODE
}

//---------------------------------------------------------------------------------------------------
bool CReceipt::Serialize(SFArchive& archive) {

    if (archive.isWriting())
        return ((const CReceipt*)this)->SerializeC(archive);

    // If we're reading a back level, read the whole thing and we're done.
    if (readBackLevel(archive))
        return true;

    // EXISTING_CODE
    // EXISTING_CODE
    archive >> contractAddress;
    archive >> gasUsed;
    archive >> logs;
    archive >> logsBloom;
    finishParse();
    return true;
}

//---------------------------------------------------------------------------------------------------
bool CReceipt::SerializeC(SFArchive& archive) const {

    // EXISTING_CODE
    // EXISTING_CODE
    // Writing always write the latest version of the data
    CBaseNode::SerializeC(archive);
    archive << contractAddress;
    archive << gasUsed;
    archive << logs;
    archive << logsBloom;

    return true;
}

//---------------------------------------------------------------------------
void CReceipt::registerClass(void) {
    static bool been_here = false;
    if (been_here) return;
    been_here = true;

    uint32_t fieldNum = 1000;
    ADD_FIELD(CReceipt, "schema",  T_NUMBER, ++fieldNum);
    ADD_FIELD(CReceipt, "deleted", T_BOOL,  ++fieldNum);
    ADD_FIELD(CReceipt, "showing", T_BOOL,  ++fieldNum);
    ADD_FIELD(CReceipt, "contractAddress", T_ADDRESS, ++fieldNum);
    ADD_FIELD(CReceipt, "gasUsed", T_GAS, ++fieldNum);
    ADD_FIELD(CReceipt, "logs", T_OBJECT|TS_ARRAY, ++fieldNum);
    ADD_FIELD(CReceipt, "logsBloom", T_BLOOM, ++fieldNum);

    // Hide our internal fields, user can turn them on if they like
    HIDE_FIELD(CReceipt, "schema");
    HIDE_FIELD(CReceipt, "deleted");
    HIDE_FIELD(CReceipt, "showing");

    // EXISTING_CODE
    // EXISTING_CODE
}

//---------------------------------------------------------------------------
SFString nextReceiptChunk_custom(const SFString& fieldIn, const void *dataPtr) {
    const CReceipt *rec = (const CReceipt *)dataPtr;
    if (rec) {
        switch (tolower(fieldIn[0])) {
            // EXISTING_CODE
            case 'i':
                // Display only the fields of this node, not it's parent type
                if ( fieldIn % "isError" )
                    return asStringU(rec->isError);
                break;
            // EXISTING_CODE
            case 'p':
                // Display only the fields of this node, not it's parent type
                if ( fieldIn % "parsed" )
                    return nextBasenodeChunk(fieldIn, rec);
                break;

            default:
                break;
        }
    }

    return "";
}

//---------------------------------------------------------------------------
bool CReceipt::handleCustomFormat(CExportContext& ctx, const SFString& fmtIn, void *dataPtr) const {
    // EXISTING_CODE
    // EXISTING_CODE
    return false;
}

//---------------------------------------------------------------------------
bool CReceipt::readBackLevel(SFArchive& archive) {

    CBaseNode::readBackLevel(archive);
    bool done = false;
    // EXISTING_CODE
    // EXISTING_CODE
    return done;
}

//---------------------------------------------------------------------------
SFArchive& operator<<(SFArchive& archive, const CReceipt& rec) {
    rec.SerializeC(archive);
    return archive;
}

//---------------------------------------------------------------------------
SFArchive& operator>>(SFArchive& archive, CReceipt& rec) {
    rec.Serialize(archive);
    return archive;
}

//---------------------------------------------------------------------------
SFString CReceipt::getValueByName(const SFString& fieldName) const {

    // Give customized code a chance to override first
    SFString ret = nextReceiptChunk_custom(fieldName, this);
    if (!ret.empty())
        return ret;

    // Return field values
    switch (tolower(fieldName[0])) {
        case 'c':
            if ( fieldName % "contractAddress" ) return fromAddress(contractAddress);
            break;
        case 'g':
            if ( fieldName % "gasUsed" ) return fromGas(gasUsed);
            break;
        case 'l':
            if ( fieldName % "logs" || fieldName % "logsCnt" ) {
                uint32_t cnt = logs.getCount();
                if (fieldName.endsWith("Cnt"))
                    return asStringU(cnt);
                if (!cnt) return "";
                SFString retS;
                for (uint32_t i = 0 ; i < cnt ; i++) {
                    retS += logs[i].Format();
                    retS += ((i < cnt - 1) ? ",\n" : "\n");
                }
                return retS;
            }
            if ( fieldName % "logsBloom" ) return fromBloom(logsBloom);
            break;
    }

    // EXISTING_CODE
    // See if this field belongs to the item's container
    ret = nextTransactionChunk(fieldName, pTrans);
    if (ret.Contains("Field not found"))
        ret = EMPTY;
    if (!ret.empty())
        return ret;
    // EXISTING_CODE

    // Finally, give the parent class a chance
    return CBaseNode::getValueByName(fieldName);
}

//-------------------------------------------------------------------------
ostream& operator<<(ostream& os, const CReceipt& item) {
    // EXISTING_CODE
    // EXISTING_CODE

    os << item.Format() << "\n";
    return os;
}

//---------------------------------------------------------------------------
const CBaseNode *CReceipt::getObjectAt(const SFString& name, uint32_t i) const {
    if ( name % "logs" && i < logs.getCount() )
        return &logs[i];
    return NULL;
}

//---------------------------------------------------------------------------
// EXISTING_CODE
// EXISTING_CODE
}  // namespace qblocks

