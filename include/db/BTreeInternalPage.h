#ifndef DB_BTREE_INTERNALPAGE_H
#define DB_BTREE_INTERNALPAGE_H

#include <db/BTreePage.h>
#include <db/BTreePageId.h>
#include <db/BTreeEntry.h>
#include <db/Field.h>
#include <cstdint>

namespace db {
    class BTreeInternalPage;
    class BTreeInternalPageIterator {
        int curEntry;
        BTreePageId *prevChildId;
        BTreePageId *currChildId;
        BTreeInternalPage *p;
    public:
        BTreeInternalPageIterator(int curEntry, BTreeInternalPage *page);

        bool operator!=(const BTreeInternalPageIterator &other) const;

        BTreeInternalPageIterator &operator++();

        BTreeEntry &operator*() const;
    };

    class BTreeInternalPageReverseIterator {
        int leftSlot;
        int rightSlot;
        BTreePageId *leftChildId;
        BTreePageId *rightChildId;
        BTreeInternalPage *p;
    public:
        BTreeInternalPageReverseIterator(int curEntry, BTreeInternalPage *page);

        bool operator!=(const BTreeInternalPageReverseIterator &other) const;

        BTreeInternalPageReverseIterator &operator++();

        BTreeEntry &operator*() const;
    };
    /**
     * Each instance of BTreeInternalPage stores data for one page of a BTreeFile and
     * implements the Page interface that is used by BufferPool.
     *
     * @see BTreeFile
     * @see BufferPool
     *
     */
    class BTreeInternalPage : public BTreePage {
        friend class BTreeInternalPageIterator;
        friend class BTreeInternalPageReverseIterator;

        uint8_t *header;
        Field **keys;
        int *children;
        int numSlots;
        BTreePageType childCategory; // either leaf or internal


        /**
         * Computes the number of bytes in the header of a B+ internal page with each entry occupying entrySize bytes
         * @return the number of bytes in the header
         */
        int getHeaderSize() const;

        /**
         * Delete the specified entry (key + 1 child pointer) from the page. The recordId
         * is used to find the specified entry, so it must not be nullptr. After deletion, the
         * entry's recordId should be set to nullptr to reflect that it is no longer stored on
         * any page.
         *
         * @param e The entry to delete
         * @param deleteRightChild - if true, delete the right child. Otherwise
         *        delete the left child
         */
        void deleteEntry(BTreeEntry *e, bool deleteRightChild);

        /**
         * Move an entry from one slot to another slot, and update the corresponding
         * headers
         */
        void moveEntry(int from, int to);

        /**
         * Abstraction to fill or clear a slot on this page.
         */
        void markSlotUsed(int i, bool value);

    protected:

        /**
         * protected method used by the iterator to get the ith key out of this page
         * @param i - the index of the key
         * @return the ith key
         */
        Field *getKey(int i);

        /**
         * protected method used by the iterator to get the ith child page id out of this page
         * @param i - the index of the child page id
         * @return the ith child page id
         */
        BTreePageId *getChildId(int i);

    public:
        /**
         * Create a BTreeInternalPage from a set of bytes of data read from disk.
         * The format of a BTreeInternalPage is a set of header bytes indicating
         * the slots of the page that are in use, some number of entry slots, and extra
         * bytes for the parent pointer, one extra child pointer (a node with m entries
         * has m+1 pointers to children), and the category of all child pages (either
         * leaf or internal).
         *  Specifically, the number of entries is equal to: <p>
         *          floor((BufferPool.getPageSize()*8 - extra bytes*8) / (entry size * 8 + 1))
         * <p> where entry size is the size of entries in this index node
         * (key + child pointer), which can be determined via the key field and
         * {@link Catalog#getTupleDesc}.
         * The number of 8-bit header words is equal to:
         * <p>
         *      ceiling((no. entry slots + 1) / 8)
         * <p>
         * @see Database#getCatalog
         * @see Catalog#getTupleDesc
         * @see BufferPool#getPageSize()
         *
         * @param id - the id of this page
         * @param data - the raw data of this page
         * @param key - the field which the index is keyed on
         */
        BTreeInternalPage(const BTreePageId &id, uint8_t *data, int key);

        /**
         * Retrieve the maximum number of entries this page can hold. (The number of keys)
          */
        int getMaxEntries() const;

        /**
         * Generates a byte array representing the contents of this page.
         * Used to serialize this page to disk.
         * <p>
         * The invariant here is that it should be possible to pass the byte
         * array generated by getPageData to the BTreeInternalPage constructor and
         * have it produce an identical BTreeInternalPage object.
         *
         * @see #BTreeInternalPage
         * @return A byte array correspond to the bytes of this page.
         */
        void *getPageData() const override;

        /**
         * Delete the specified entry (key + right child pointer) from the page. The recordId
         * is used to find the specified entry, so it must not be null. After deletion, the
         * entry's recordId should be set to null to reflect that it is no longer stored on
         * any page.
         * @param e The entry to delete
         */
        void deleteKeyAndRightChild(BTreeEntry *e);

        /**
         * Delete the specified entry (key + left child pointer) from the page. The recordId
         * is used to find the specified entry, so it must not be null. After deletion, the
         * entry's recordId should be set to null to reflect that it is no longer stored on
         * any page.
         * @param e The entry to delete
         */
        void deleteKeyAndLeftChild(BTreeEntry *e);


        /**
         * Update the key and/or child pointers of an entry at the location specified by its
         * record id.
         * @param e - the entry with updated key and/or child pointers
         */
        void updateEntry(BTreeEntry *e);

        /**
         * Adds the specified entry to the page; the entry's recordId should be updated to
         * reflect that it is now stored on this page.
         * @param e The entry to add.
         */
        void insertEntry(BTreeEntry e);

        /**
         * Returns the number of entries (keys) currently stored on this page
         */
        int getNumEntries() const;

        /**
         * Returns the number of empty slots on this page.
         */
        int getNumEmptySlots() const override;

        /**
         * Returns true if associated slot on this page is filled.
         */
        bool isSlotUsed(int i) const override;

        /**
         * @return an iterator over all entries on this page
         * (note that this iterator shouldn't return entries in empty slots!)
         */
        BTreeInternalPageIterator begin();

        BTreeInternalPageIterator end();

        BTreeInternalPageReverseIterator rbegin();

        BTreeInternalPageReverseIterator rend();
    };
}

#endif
