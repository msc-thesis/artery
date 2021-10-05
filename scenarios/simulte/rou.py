import click
import lxml.etree as etree
from copy import deepcopy

@click.command()
@click.option("-r", "--route_file", required=True, type=str, help="SUMO route file to parse")
@click.option("-v", "--num_vehicles", required=True, type=int, help="Number of vehicles to insert")
@click.option("-d", "--delay", required=True, type=int, help="Number of seconds to wait between vehicles")
def run(route_file, num_vehicles, delay):
    tree = etree.parse(route_file)
    root = tree.getroot()

    for i in range(1, num_vehicles):
        tmp = deepcopy(root[0])
        tmp.attrib["id"] = str(i)
        tmp.attrib["depart"] = f"{i * delay}.00"
        root.append(tmp)
    
    xmlstr = etree.tostring(root, pretty_print=True, xml_declaration=True, encoding="utf-8")

    with open(route_file.replace(".template", ""), "wb") as f:
        f.write(xmlstr)

if __name__ == "__main__":
    run()
