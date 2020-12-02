/*
 * Copyright 2019 garciafa
 * This file is part of PprzLinkCPP
 *
 * PprzLinkCPP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PprzLinkCPP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ModemTester.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/** \file Message.h
 *
 *
 */

#ifndef PPRZLINKCPP_MESSAGE_H
#define PPRZLINKCPP_MESSAGE_H

#include <map>
#include <cassert>
#include <pprzlink/MessageDefinition.h>
#include <pprzlink/exceptions/pprzlink_exception.h>
#include <pprzlink/FieldValue.h>
#include <variant>

namespace pprzlink {
  /**
   *
   */
  class Message {
  public:
    /**
     *
     * @param def
     */
    explicit Message(const MessageDefinition &def);

    /**
     *
     * @tparam ValueType
     * @param name
     * @param value
     */
    template<typename ValueType>
    void addField(const std::string &name, ValueType value)
    {
      auto field = def.getField(name); // Will throw no_such_field if non existing field name
      fieldValues[name] = FieldValue(field,value);
    }

    /**
     *
     * @tparam ValueType
     * @param name
     * @param value
     */
    template<typename ValueType>
    void getField(const std::string &name, ValueType &value) const
    {
      auto field = def.getField(name); // Will throw no_such_field if non existing field name
      auto const &it = fieldValues.find(name);
      if (it == fieldValues.end())
      {
        throw field_has_no_value("In message " + def.getName() + " field " + name + " has not value !");
      }
      it->second.getValue(value);
    }

    /**
     *
     * @tparam ValueType
     * @param index
     * @param value
     */
    template<typename ValueType>
    void getField(size_t index, ValueType &value) const
    {
      auto name = def.getField(index).getName();
      auto const &it = fieldValues.find(name);
      if (it == fieldValues.end())
      {
        throw field_has_no_value("In message " + def.getName() + " field " + name + " has not value !");
      }
      it->second.getValue(value);
    }

    /**
     *
     * @param index
     * @param buffer
     * @param offset
     */
    void addFieldFromBuffer(size_t index, BytesBuffer const &buffer, size_t & offset);

    /**
     *
     * @param index
     * @param buffer
     * @return
     */
    size_t addFieldToBuffer(size_t index, BytesBuffer &buffer) const;

    /**
     *
     * @param index
     * @return
     */
    [[nodiscard]] const FieldValue& getRawValue(int index) const;

    /**
     *
     * @param name
     * @return
     */
    [[nodiscard]] const FieldValue& getRawValue(const std::string& name) const;

    /**
     *
     * @return
     */
    [[nodiscard]] size_t getNbValues() const;

    /**
     *
     * @return
     */
    [[nodiscard]] const MessageDefinition &getDefinition() const;

    /**
     *
     * @return
     */
    [[nodiscard]] std::string toString() const;

    /**
     *
     * @return
     */
    const std::variant<std::string, uint8_t> &getSenderId() const;

    /**
     *
     * @return
     */
    uint8_t getReceiverId() const;

    /**
     *
     * @return
     */
    uint8_t getComponentId() const;

    /**
     *
     * @return
     */
    uint8_t getClassId() const;

    /**
     *
     * @param senderId
     */
    void setSenderId(const std::variant<std::string, uint8_t> &senderId);

    /**
     *
     * @param receiverId
     */
    void setReceiverId(uint8_t receiverId);

    /**
     *
     * @param componentId
     */
    void setComponentId(uint8_t componentId);

    /**
     *
     * @return  the size of the message in bytes if stored in binary
     */
    size_t getByteSize() const;

  private:
    MessageDefinition def;
    std::map<std::string, FieldValue> fieldValues;
    std::variant<std::string,uint8_t> sender_id;
    uint8_t receiver_id;
    uint8_t component_id;
  };

}
#endif //PPRZLINKCPP_MESSAGE_H
