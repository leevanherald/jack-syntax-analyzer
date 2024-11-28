import xml.etree.ElementTree as ET

def print_xml_tree(element, level=0, prefix=""):
    # Prefix the tag with appropriate tree lines
    if level == 0:
        print(f"{element.tag}")
    else:
        connector = "└──" if prefix.endswith("│   ") else "├──"
        print(f"{prefix}{connector} {element.tag}")

    # Print attributes, if any
    if element.attrib:
        for attr, value in element.attrib.items():
            print(f"{prefix}    @{attr} = {value}")
    
    # Handle children elements
    children = list(element)
    for i, child in enumerate(children):
        is_last = (i == len(children) - 1)
        child_prefix = prefix + ("    " if is_last else "│   ")
        print_xml_tree(child, level + 1, child_prefix)

    # Print text content if available and non-empty
    if element.text and element.text.strip():
        print(f"{prefix}    {element.text.strip()}")

# Function to read the XML from a file
def read_xml_from_file(file_path):
    with open(file_path, 'r') as file:
        xml_data = file.read()
    return ET.fromstring(xml_data)

# Example usage with a file
file_path = "output.xml"  # Replace this with the actual path to your XML file

# Read and parse XML from the file
root = read_xml_from_file(file_path)

# Print the tree structure
print_xml_tree(root)
