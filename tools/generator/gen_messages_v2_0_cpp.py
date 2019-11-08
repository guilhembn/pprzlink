#!/usr/bin/env python
'''
parse a PPRZLink protocol XML file and generate a CPP implementation
for version 2.0 of the protocol

Copyright (C) 2017 Fabien Garcia <fabien.garcia@enac.fr>
For the Paparazzi UAV and PPRZLINK projects

based on:
    Copyright Andrew Tridgell 2011
    Released under GNU GPL version 3 or later
'''

from __future__ import print_function
import sys, os
import gen_messages, pprz_template, pprz_parse

t = pprz_template.PPRZTemplate()

header_extension=".hpp"

def generate_main_h(directory, name, xml):
    '''generate main header per XML file'''
    f = open(os.path.join(directory, name), mode='w')
    xml.header_extension=header_extension
    t.write(f, '''
/** @file
 *  @brief PPRZLink message header built from ${filename}
 *  @see http://paparazziuav.org
 */

#ifndef PPRZLINK_DEFAULT_VER
#define PPRZLINK_DEFAULT_VER 2
#endif

${{message:#include "${class_name}/${msg_name}${header_extension}"
}}
''', xml)

    f.close()


def copy_fixed_headers(directory, protocol_version):
    '''copy the fixed protocol headers to the target directory'''
    import shutil
    # TODO: List all global headers and make them hpp
    hlist = [ 'pprzlink_messages.hpp', 'tuple_iteration.hpp' ]
    basepath = os.path.dirname(os.path.realpath(__file__))
    srcpath = os.path.join(basepath, 'C++/include_v%s' % protocol_version)
    if directory == '':
        print("Skip copying fixed headers")
        return
    print("Copying fixed headers")
    for h in hlist:
        src = os.path.realpath(os.path.join(srcpath, h))
        dest = os.path.realpath(os.path.join(directory, h))
        if src == dest:
            continue
        shutil.copy(src, dest)

def generate_one(directory, xml, m):
    f = open(os.path.join(os.path.join(directory, xml.class_name), m.msg_name + header_extension), mode='w')
    t.write(f, '''
/** @file
 *  @brief PPRZLink message header for ${msg_name} in class ${class_name}
 *
 *  ${description}
 *  @see http://paparazziuav.org
 */
#ifndef _VAR_MESSAGES_${class_name}_${msg_name}_HPP_
#define _VAR_MESSAGES_${class_name}_${msg_name}_HPP_

#include "pprzlink_messages.hpp"

namespace pprzlink {
  namespace ${class_name} {
    class ${msg_name} : public PprzMessage<${{fields:${type},}}> {
    public:
    enum fields {${{fields:
      ${field_enum_name}, // ${description}}} 
    };

    ${{fields:
      /** Getter for field ${field_name} in message ${msg_name}
        * @return ${description}
        */
      inline ${type}& get_${field_name}() {return get<${field_enum_name}>();}

}}
      const static MessageId msg_id;
    };
    const MessageId ${msg_name}::msg_id={"${msg_name}",${id},"${class_name}",${class_id}};
  }
}
#endif // _VAR_MESSAGES_${class_name}_${msg_name}_HPP_
''', {'msg_name' : m.msg_name, 'description' : m.description ,'class_id' : xml.class_id, 'class_name' : xml.class_name, 'id' : m.id, 'fields' : m.fields, 'message' : xml.message})


def generate(output, xml):
    '''generate complete pprzlink C++ implemenation'''

    directory, name = os.path.split(output)
    print("Generating C++ implementation in %s" % output)
    if directory != '':
        pprz_parse.mkdir_p(directory)
        pprz_parse.mkdir_p(os.path.join(directory, xml.class_name))

    # add some extra field attributes for convenience with arrays
    for m in xml.message:
        for f in m.fields:
            # Prevent problems due to some fields names
            f.field_enum_name = 'field_'+f.field_name
            if f.array_type == 'VariableArray':
                f.type = 'std::vector<'+f.type+'> '
            elif f.array_type == 'FixedArray':
                f.type = 'std::array<f.type,'+f.array_length+'>'
            elif f.type == 'string':
                f.type = 'std::string'
        generate_one(directory, xml, m)

    generate_main_h(directory, name, xml)
    copy_fixed_headers(directory, xml.protocol_version)
