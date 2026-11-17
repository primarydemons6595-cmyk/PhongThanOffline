case PHONGTHAN_GDI_ITEM_NETWORK_ID:
    if (IsValidClientItemIndex((unsigned int)nParam))
        nRet = (int)Item[nParam].GetID();
    break;
