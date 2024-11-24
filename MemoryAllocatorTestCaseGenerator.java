import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashSet;
import java.util.Random;
import java.util.Set;

public class MemoryAllocatorTestCaseGenerator {
    public static void main(String[] args) {
        // Define system memory sizes (in MB)
        int[] memorySizes = {1, 2, 4, 8, 16};
        String[] strategies = {"F", "B", "W"}; // First Fit, Best Fit, Worst Fit
        int numProcesses = 10; // Number of processes to generate for each test case
        int maxMemoryRequest = 512; // Max memory each process can request (in KB)

        // Create "inputs" folder if it does not exist
        File folder = new File("inputs");
        if (!folder.exists()) {
            folder.mkdir();
            System.out.println("Created 'inputs' directory.");
        }

        // Generate test cases for each memory size and strategy
        for (int memorySize : memorySizes) {
            for (String strategy : strategies) {
                String fileName = "inputs/test_" + memorySize + "MB_" + strategy + ".txt";
                try (FileWriter writer = new FileWriter(fileName)) {
                    // Write total memory (converted to KB)
                    int totalMemoryKB = memorySize * 1024;
                    writer.write(totalMemoryKB + "\n");

                    Random rand = new Random();
                    Set<Integer> allocatedProcesses = new HashSet<>(); // Set to keep track of allocated processes
                    String lastOperation = ""; // Track the last operation to avoid consecutive 'C'

                    for (int i = 1; i <= numProcesses; i++) {
                        // Randomly choose between allocation, release, and compact
                        int operation = rand.nextInt(3);

                        if (operation == 0) {
                            // Memory allocation request
                            int requestSize = rand.nextInt(maxMemoryRequest) + 1; // 1KB to maxMemoryRequest
                            writer.write("RQ P" + i + " " + requestSize + " " + strategy + "\n");
                            allocatedProcesses.add(i); // Add this process to the allocated set
                            lastOperation = "RQ"; // Update last operation
                        } else if (operation == 1) {
                            // Memory release (randomly choose a process to release if it has been allocated memory)
                            int processToRelease = rand.nextInt(i + 1); // Randomly choose process ID
                            if (allocatedProcesses.contains(processToRelease)) {
                                writer.write("RL P" + processToRelease + "\n");
                                allocatedProcesses.remove(processToRelease); // Remove the process from the allocated set
                                lastOperation = "RL"; // Update last operation
                            }
                        } else {
                            // Compact memory (but not consecutively)
                            if (!lastOperation.equals("C")) {
                                writer.write("C\n");
                                lastOperation = "C"; // Update last operation
                            } else {
                                // Skip writing the 'C' operation if the last operation was already 'C'
                                i--; // Decrease counter to ensure we don't skip a test case
                            }
                        }
                    }

                    // Add a final memory status check
                    writer.write("STAT\n");

                    System.out.println("Test case written to " + fileName);
                } catch (IOException e) {
                    System.err.println("Error writing to file " + fileName);
                    e.printStackTrace();
                }
            }

            // Generate additional "mixed" test case for each memory size
            String fileName = "inputs/test_" + memorySize + "MB_mixed.txt";
            try (FileWriter writer = new FileWriter(fileName)) {
                // Write total memory (converted to KB)
                int totalMemoryKB = memorySize * 1024;
                writer.write(totalMemoryKB + "\n");

                Random rand = new Random();
                Set<Integer> allocatedProcesses = new HashSet<>(); // Set to keep track of allocated processes
                String lastOperation = ""; // Track the last operation to avoid consecutive 'C'

                for (int i = 1; i <= numProcesses; i++) {
                    // Randomly choose between allocation, release, and compact
                    int operation = rand.nextInt(3);

                    if (operation == 0) {
                        // Memory allocation request
                        int requestSize = rand.nextInt(maxMemoryRequest) + 1; // 1KB to maxMemoryRequest
                        String strategy = getRandomStrategy(rand); // Choose random strategy (F, B, W)
                        writer.write("RQ P" + i + " " + requestSize + " " + strategy + "\n");
                        allocatedProcesses.add(i); // Add this process to the allocated set
                        lastOperation = "RQ"; // Update last operation
                    } else if (operation == 1) {
                        // Memory release (randomly choose a process to release if it has been allocated memory)
                        int processToRelease = rand.nextInt(i + 1); // Randomly choose process ID
                        if (allocatedProcesses.contains(processToRelease)) {
                            writer.write("RL P" + processToRelease + "\n");
                            allocatedProcesses.remove(processToRelease); // Remove the process from the allocated set
                            lastOperation = "RL"; // Update last operation
                        }
                    } else {
                        // Compact memory (but not consecutively)
                        if (!lastOperation.equals("C")) {
                            writer.write("C\n");
                            lastOperation = "C"; // Update last operation
                        } else {
                            // Skip writing the 'C' operation if the last operation was already 'C'
                            i--; // Decrease counter to ensure we don't skip a test case
                        }
                    }
                }

                // Add a final memory status check
                writer.write("STAT\n");

                System.out.println("Test case written to " + fileName);
            } catch (IOException e) {
                System.err.println("Error writing to file " + fileName);
                e.printStackTrace();
            }
        }
    }
    // ************************************************************************************************
    // Method to get a random strategy (F, B, or W)
    private static String getRandomStrategy(Random rand) {
        int strategyChoice = rand.nextInt(3);
        if (strategyChoice == 0) {
            return "F"; // First Fit
        } else if (strategyChoice == 1) {
            return "B"; // Best Fit
        } else {
            return "W"; // Worst Fit
        }
    }
}

// ************************************************************************************************
/* 
  Compile the code
    javac MemoryAllocatorTestCaseGenerator.java

  Run the code
    java MemoryAllocatorTestCaseGenerator
*/