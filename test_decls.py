from pygccxml import utils
from pygccxml import declarations
from pygccxml import parser
import argparse


# You can use this to parse different declarations in c++ files


def parse_filename(source_file, output_file):
    # Find the location of the xml generator (castxml or gccxml)
    generator_path, generator_name = utils.find_xml_generator()

    # Configure the xml generator
    xml_generator_config = parser.xml_generator_configuration_t(
        xml_generator_path=generator_path,
        xml_generator=generator_name)

    xml_generator_config.cflags = "-std=c++11"

    # The c++ file we want to parse
    filename = "test.cpp"

    # Parse the c++ file
    decls = parser.parse([filename], xml_generator_config)


    global_namespace = declarations.get_global_namespace(decls)

    # Get access to the global namespace
    f = declarations.calldef_matcher(name="f")

    # Get access to the 'ns' namespace
    func = declarations.matcher.get_single(f, decls)

    print(dir(func))
    print(func.location.line)
    print(func.required_args)
    print(func)

    for arg in func.arguments:
        print(
            arg.name,
            str(arg.decl_type),
            declarations.is_std_string(arg.decl_type),
            declarations.is_reference(arg.decl_type))

    for d in global_namespace.declarations:

        if isinstance(d, declarations.calldef_t) and d.location.file_name == filename:
            print("MYFUNC %s\t%s" % (d.name, d.location.file_name
                                     ))
    # dd = declarations.calldef_matcher(header_file="/home/horrid/Documents/Dissertation/Distributed-CSP/test.cpp")
    # dd1 = declarations.matcher.get_single
    # print(dd)


if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument("-s", "--source_file", help="C++ source file to parse")
    argparser.add_argument("-o", "--output_file", help="name of the output file")
    args = argparser.parse_args()
    parse_filename(args.source_file, args.output_file)

