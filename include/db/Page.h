#ifndef DB_PAGE_H
#define DB_PAGE_H

#include <db/PageId.h>
#include <db/TransactionId.h>
#include <optional>

namespace db {
    /**
     * Page is the interface used to represent pages that are resident in the
     * BufferPool.  Typically, DbFiles will read and write pages from disk.
     *
     * Pages may be "dirty", indicating that they have been modified since they
     * were last written out to disk.
     */
    class Page {
    protected:
        std::optional<TransactionId> dirty = std::nullopt;
    public:
        /**
         * Return the id of this page.  The id is a unique identifier for a page
         * that can be used to look up the page on disk or determine if the page
         * is resident in the buffer pool.
         *
         * @return the id of this page
         */
        virtual const PageId &getId() const = 0;

        /**
         * Get the id of the transaction that last dirtied this page, or nullopt if the page is clean.
         *
         * @return The id of the transaction that last dirtied this page, or nullopt
         */
        std::optional<TransactionId> isDirty() const {
            return dirty;
        }

        /**
         * Set the dirty state of this page as dirtied by a particular transaction
         */
        virtual void markDirty(std::optional<TransactionId> tid) final {
            dirty = tid;
        }
        /**
         * Generates a byte array representing the contents of this page.
         * Used to serialize this page to disk.
         *
         * The invariant here is that it should be possible to pass the byte array
         * generated by getPageData to the Page constructor and have it produce
         * an identical Page object.
         *
         * @return A byte array correspond to the bytes of this page.
         */
        virtual void *getPageData() const = 0;

        virtual ~Page() = default;
    };
}

#endif
