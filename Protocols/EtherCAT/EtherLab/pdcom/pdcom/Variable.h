/*****************************************************************************
 * vim:tw=78
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/** @file */

#ifndef PDCOM_VARIABLE_H
#define PDCOM_VARIABLE_H

#include <string>
#include <vector>
#include <ostream>
#include <istream>
#include <stdint.h>

namespace PdCom {

class Process;

/** Process variable
 *
 * This class abstracts the process variable.
 */
class Variable {
    public:
        //////////////////////////////////////////////////////////////////
        /** Variable's type */
        enum Type {
            boolean_T = 0,
            uint8_T,  int8_T,
            uint16_T, int16_T,
            uint32_T, int32_T,
            uint64_T, int64_T,
            double_T, single_T
        };

        //////////////////////////////////////////////////////////////////
        /** Subscription class
         *
         * The library passes on this class to Subscriber as a result of a
         * successful variable \ref Process::subscribe "subscribe()".
         *
         * This class is used to query various properties of the Subscription
         * as well as a method to get the variable's value in any data type by
         * calling getValue(), getStringValue() and print().
         *
         * The value of a subscription is always accessible (doesn't SEGV),
         * but it is not always sensible to call it outside of
         * Subscriber::newValue() or Subscriber::newGroupValue().
         *
         * To cancel a subscription, issue cancel() on it. cancel() is safe
         * from within \ref Subscriber::newValue "newValue()" or \ref
         * Subscriber::newGroupValue "newGroupValue()".
         *
         * Signals that are subscribed with an intermittent \c
         * interval&nbsp;&lt;&nbsp;0 are only updated after issuing poll().
         */
        class Subscription {
            public:
                /** Constructor */
                Subscription(Process* process,
                        int id,
                        const Variable* v,
                        int decimation,
                        const uint64_t* const* time,
                        const char* const* data);

                Process* const process; /**< pointer to process */
                const int id;   /**< id supplied during Process::subscribe() */
                const Variable* const variable; /**< Actual variable */
                const int decimation;   /**< Subscription decimation */

                /** Time of value in nanoseconds since epoch. Use double
                 * dereferencing to access the value, eg.
                 * \code
                 *    double time = 1.0e-9 * **subscription->time_ns;
                 * \endcode
                 */
                const uint64_t* const* const time_ns;

                /** Cancel subscription
                 *
                 * Calling cancel() is safe from within Subscriber::newValue()
                 * and Subscriber::newGroupValue().
                 */
                virtual void cancel() const = 0;

                /** Poll a subscription.
                 *
                 * This is only available for signals when request \c
                 * interval&nbsp;&lt;&nbsp;0, otherwise an error value is
                 * returned.
                 *
                 * The value is available during Subscriber::newValue().
                 *
                 * \return true on failure
                 */
                bool poll() const;

                /** Retrieve subscriptions value in any data type
                 *
                 * @param val value buffer to write to. Must be at least \p
                 * nelem long
                 * @param index starting index
                 * @param nelem number of elements to retrieve
                 */
                template <typename T>
                    void getValue(T* val, size_t index, size_t nelem) const;

                /** Get the entire value as a linear string.
                 *
                 * @param delimiter delimiter between vector elements
                 */
                std::string getStringValue(char delimiter = ',') const;

                /** Write the value to std::ostream
                 *
                 * This allows the user finer control of double variables.
                 *
                 * @param os std::ostream output stream
                 * @param delimiter delimiter between vector elements
                 */
                void print(std::ostream& os, char delimiter) const;

            protected:
                ~Subscription();

            private:

                const char* const * const data;
        };

        //////////////////////////////////////////////////////////////////
        /** Constructor */
        Variable( Type type,
                double sampleTime,
                size_t task,
                bool writeable,
                const std::string& alias,
                size_t ndim,
                const size_t *dims);

        virtual ~Variable();

        const Type type;          /**< Data type */
        const char* const ctype;  /**< c-type string
                                   * One of:
                                   *   *  "uint8_t"
                                   *   *   "int8_t"
                                   *   * "uint16_t"
                                   *   *  "int16_t"
                                   *   * "uint32_t"
                                   *   *  "int32_t"
                                   *   * "uint64_t"
                                   *   *  "int64_t"
                                   *   *   "double"
                                   *   *    "float"
                                   */
        const double sampleTime;  /**< sample time of variable; 0 for
                                   * parameters */
        const size_t task;        /**< Task id of signals. Note that signals 
                                   * of the same task may have different
                                   * sample times, indicating that the signal
                                   * is subrated within the task */
        const bool writeable;     /**< Can be written to, for parameters */

        const std::string alias;  /**< Optional alias */

        /** Short name without leading path */
        virtual std::string name() const = 0;

        /** complete path, including name() */
        virtual std::string path() const = 0;

        const size_t nelem;     /**< Number of elements */
        const std::vector<size_t> dim;  /**< Dimension array */
        const size_t typeWidth; /**< sizeof(datatype) */
        const size_t bytes;     /**< memory required to store everything */

        /** Scalar test */
        bool isScalar() const;

        /** Test for a vector.
         *
         * A scalar is also a vector with one element
         *
         * \return Number of vector elements */
        size_t isVector() const;

        /** Set a parameter's value.
         *
         * Alias for read().
         */
        bool setStringValue(const std::string& value) const;


        /** Set a parameter's value
         *
         * @param val value buffer to read from. Must have at lease \p nelem
         * elements
         * @param offset Starting index
         * @param nelem number of elements to write
         *
         * \return true on error
         */
        template <typename T>
            bool setValue(const T* val,
                    size_t offset, size_t nelem) const;

        /** Set a parameter's value
         *
         * @param is a std::istream value. Elements must be delimited by any
         * non-numeric character.
         *
         * \return true on error
         */
        bool read(std::istream& is) const;

    protected:

        /** \cond INTERNAL */

        /** for internal use */
        virtual bool poll(const Subscription*) const { return true; }

        /** for internal use */
        virtual bool setValue(const void*, size_t /*UserTypeIdx*/,
                size_t /*index*/, size_t /*nelem*/) const {
            return true;
        }

        /** for internal use */
        void copyValue(char* dst, const void* src,
                size_t UserTypeIdx, size_t n) const;

        /** for internal use */
        void printValue(std::ostream& os, const char* buf, char delim) const;

        /** \endcond */

    private:

        typedef void (Variable::*GetValueFunc_T)
            (void* dst, size_t nelem, const char* src, size_t offset) const;
        const GetValueFunc_T*  const getValueFunc;

        typedef void (Variable::*CopyValueFunc_T)
            (char* dst, const void* src, size_t nelem) const;
        const CopyValueFunc_T* const copyValueFunc;

        typedef void (Variable::*PrintValueFunc_T)
            (std::ostream& os, const char* data, char delim) const;
        const PrintValueFunc_T printValueFunc;

        template <class CType>
            void printFcn(
                    std::ostream& os, const char* buf, char delim) const;
        static const PrintValueFunc_T printValueFuncList[];

        template <class CType>
            bool readFcn(std::istream& is) const;
        typedef bool (Variable::*ReadValueFunc_T)
            (std::istream& is) const;
        const ReadValueFunc_T readValueFunc;
        static const ReadValueFunc_T readValueFuncList[];

        template <class CType, class UserType>
            void copyValue(char* dst, const void* src, size_t nelem) const;
        template <class CType, class UserType>
            void getValue(void* dst, size_t nelem,
                    const char* src, size_t offset) const;
        template <class CType>
            struct DataConverter {
                static const GetValueFunc_T getValueFunc[];
                static const CopyValueFunc_T copyValueFunc[];
            };
        static const GetValueFunc_T*  const getValueFuncList[];
        static const CopyValueFunc_T* const copyValueFuncList[];
};

}

/** Stream operator to dump a Subscription to std::ostream */
std::ostream& operator<< (std::ostream& os,
        const PdCom::Variable::Subscription& subscription);

/** Stream operator to write a value to a Variable */
std::istream& operator>> (std::istream& is,
        const PdCom::Variable& variable);

#endif //PDCOM_VARIABLE_H
