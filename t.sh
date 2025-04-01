for ((i=0; i<=256; i++)); do
    echo "int$i:"
    echo -e "\tcli"
    echo -e "\tcld"
    echo -e "\tPUSH_ALL_REGS"
    echo -e "\tmov rsi, $i"
    echo -e "\tjmp intx"
    echo  # Add a blank line between each set of instructions
done
