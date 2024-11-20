import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

public class TestCaseGenerator {
    private static final int[] SYSTEM_MEMORIES = {1, 2, 4, 8, 16}; // in MB
    private static final String[] ALLOCATION_METHODS = {"F", "B", "W"};
    private static final int MAX_PROCESSES = 10;
    private static final int MAX_PROCESS_SIZE = 512; // in KB
    private static final int OPERATIONS_PER_TEST = 20;

    public static void main(String[] args) {
        createInputsFolder();
        generateTestCases();
    }

    private static void createInputsFolder() {
        File inputsFolder = new File("inputs");
        if (!inputsFolder.exists()) {
            inputsFolder.mkdir();
        }
    }

    private static void generateTestCases() {
        Random random = new Random();

        for (int memory : SYSTEM_MEMORIES) {
            for (String allocationMethod : ALLOCATION_METHODS) {
                String fileName = String.format("inputs/test_case_%dMB_%s.txt", memory, allocationMethod);
                try (FileWriter writer = new FileWriter(fileName)) {
                    // Write system memory and allocation method
                    writer.write(memory + "\n");
                    writer.write(allocationMethod + "\n");

                    // Generate operations
                    for (int i = 0; i < OPERATIONS_PER_TEST; i++) {
                        int operation = random.nextInt(4);
                        switch (operation) {
                            case 0: // Request memory
                                int processId = random.nextInt(MAX_PROCESSES) + 1;
                                int size = random.nextInt(MAX_PROCESS_SIZE) + 1;
                                writer.write(String.format("RQ P%d %d\n", processId, size));
                                break;
                            case 1: // Release memory
                                processId = random.nextInt(MAX_PROCESSES) + 1;
                                writer.write(String.format("RL P%d\n", processId));
                                break;
                            case 2: // Compact
                                writer.write("C\n");
                                break;
                            case 3: // Status
                                writer.write("STAT\n");
                                break;
                        }
                    }
                } catch (IOException e) {
                    System.err.println("Error writing to file: " + fileName);
                    e.printStackTrace();
                }
            }
        }
    }
}