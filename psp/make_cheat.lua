-- Assemble the source
os.execute("armips trae.s")

-- Read the output file
local f = io.open("output.bin", "rb")

local size = f:seek("end")
f:seek("set")

-- The base address of our instructions
--local base = 0x88bf29c - 0x08800000
local base = 0

-- Read all instructions and print as _L
for i=0, size - 4, 4 do
    local data = string.unpack("<I", f:read(4))

    print(string.format("_L 0x2%07X 0x%08X", base + i, data))
end

f:close()