#include <Wire.h>        // Include I2C communication library
#include <at24c256.h>    // Include library for AT24C256 EEPROM

// Create EEPROM object with I2C address 0x50 (A0, A1, A2 pins tied to GND)
AT24C256 eeprom(0x50);

// Define EEPROM memory locations
int spAddr = 0;   // Address 0 used to store the Stack Pointer (SP)
int baseAddr = 2; // Stack data starts from address 2 in EEPROM

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud
  Wire.begin();        // Initialize I2C communication
  Serial.println("📟 Stack Calculator using AT24C256");

  // Check if stack pointer is uninitialized (EEPROM default = 255)
  if (eeprom.read(spAddr) == 255) {
    eeprom.write(spAddr, 0);   // Initialize stack pointer to 0
    delay(5);                  // Allow EEPROM write time
    Serial.println("Stack pointer initialized to 0.");
  } else {
    // If already initialized, restore last saved stack pointer
    Serial.print("Restored stack pointer: ");
    Serial.println(eeprom.read(spAddr));
  }
}

// ---------- Stack Pointer Control ----------

// Reads current stack pointer (SP) from EEPROM
int getSP() {
  return eeprom.read(spAddr);
}

// Updates stack pointer (SP) value in EEPROM
void setSP(int sp) {
  eeprom.write(spAddr, sp);
  delay(5);
}

// ---------- Stack Operations ----------

// Pushes a new integer value onto the stack
void push(int value) {
  int sp = getSP();  // Get current SP value
  // Store the new value at calculated EEPROM location
  eeprom.put(baseAddr + sp * sizeof(int), value);
  delay(5);           // Wait for EEPROM write
  setSP(sp + 1);      // Increment stack pointer
}

// Pops (removes) top value from the stack and returns it
int pop() {
  int sp = getSP() - 1; // Move SP to previous position
  if (sp < 0) {         // If SP < 0, stack underflow
    Serial.println("⚠️ Stack underflow!");
    return 0;
  }
  int value;
  // Read top value from EEPROM
  eeprom.get(baseAddr + sp * sizeof(int), value);
  setSP(sp);  // Update SP
  return value;
}

// Prints top value without popping it
void printTop() {
  int sp = getSP();
  if (sp == 0) {  // Stack empty
    Serial.println("Stack empty!");
    return;
  }
  int value;
  // Get top value (SP - 1)
  eeprom.get(baseAddr + (sp - 1) * sizeof(int), value);
  Serial.print("Top: ");
  Serial.println(value);
}

// Displays all stack contents (bottom → top)
void showStack() {
  int sp = getSP();
  if (sp == 0) {
    Serial.println("Stack empty!");
    return;
  }
  Serial.println("🧱 Stack contents (bottom → top):");
  for (int i = 0; i < sp; i++) {
    int value;
    eeprom.get(baseAddr + i * sizeof(int), value);
    Serial.print(i);
    Serial.print(": ");
    Serial.println(value);
  }
}

// Clears the entire stack memory
void clearStack() {
  int sp = getSP();
  // Overwrite all stack values with 0
  for (int i = 0; i < sp; i++) {
    eeprom.put(baseAddr + i * sizeof(int), 0);
    delay(5);
  }
  setSP(0);  // Reset SP to 0
  Serial.println("🧹 Stack cleared!");
}

// ---------- Main Program Loop ----------
void loop() {
  if (Serial.available()) {        // If user sends a command
    char cmd = Serial.read();      // Read one character command

    // Arithmetic operations
    if (cmd == '+') {
      int a = pop();
      int b = pop();
      push(b + a);
      Serial.println(b + a);
    } 
    else if (cmd == '-') {
      int a = pop();
      int b = pop();
      push(b - a);
      Serial.println(b - a);
    } 
    else if (cmd == '*') {
      int a = pop();
      int b = pop();
      push(b * a);
      Serial.println(b * a);
    } 
    else if (cmd == '/') {
      int a = pop();
      int b = pop();
      if (a == 0) {  // Handle divide by zero
        Serial.println("⚠️ Divide by zero!");
        push(b); // Restore b back to stack
      } else {
        push(b / a);
        Serial.println(b / a);
      }
    } 

    // Stack management commands
    else if (cmd == 'p') {  // Push a new value
      while (!Serial.available()); // Wait for user input
      int val = Serial.parseInt(); // Read integer
      push(val);
      Serial.print("Pushed: ");
      Serial.println(val);
    } 
    else if (cmd == 't') {  // Print top value
      printTop();
    }
    else if (cmd == 's') {  // Show all stack contents
      showStack();
    }
    else if (cmd == 'c') {  // Clear stack
      clearStack();
    }
  }
}
