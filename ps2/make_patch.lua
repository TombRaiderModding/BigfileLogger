-- Assemble the source
os.execute("armips tr8.s")

-- Read the output file
local f = io.open("output.bin", "rb")

local size = f:seek("end")
f:seek("set")

-- The base address of our instructions
local base = 0x11b488

-- Read all instructions and print as patch=
for i=0, size - 4, 4 do
    local data = string.unpack("<I", f:read(4))

    print(string.format("patch=0,EE,2%07X,extended,%08X", base + i, data))
end

f:close()