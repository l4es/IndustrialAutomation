import pydax
import time

pydax.init("PyDAX")

new_type = (("Mem1", "BOOL", 10),
            ("Mem2", "BOOL", 1),
            ("Mem3", "BOOL", 3))

x = pydax.cdt_create("PyDAX_Type", new_type)
#print hex(x)

pydax.add("PyBYTE", "BYTE", 10)
pydax.add("PySINT", "INT", 10)
pydax.add("PyINT", "INT", 10)
pydax.add("PyINT", "INT", 10)
pydax.add("PyBOOL", "BOOL", 10)

pydax.add("PyCDTTAG", "PyDAX_TYPE", 1)

for n in range(1000):
  print pydax.read("PyCDTTAG", 0)
  time.sleep(1)
#print pydax.get("PyDAXTAG")
#print pydax.get(0)
#print pydax.get(1)
#print pydax.get(2)

#print pydax.cdt_get("PyDAX_Type")

#print pydax.read("PyBYTE", 0)
#print pydax.read("PyBOOL[0]", 1)
#print pydax.read("PyBOOL", 0)


pydax.free()
