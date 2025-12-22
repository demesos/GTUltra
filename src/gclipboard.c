//
// GTUltra Clipboard Functions for Notes-Only Export/Import
//

#include "goattrk2.h"
#include <SDL/SDL.h>

// External references
extern char *notename[];
extern int stepsize;
extern unsigned char pattern[MAX_PATT][MAX_PATTROWS*4+4];
extern int pattlen[MAX_PATT];

// Parse a note string like "C-4" or "D#5" or "---" into a note value
// Returns note value (0x60-0xbc) or REST (0xbd) or 0 for invalid
unsigned char parseNoteString(const char *str)
{
    // Handle empty string
    if (!str || str[0] == '\0')
        return 0;
    
    // Handle rest/empty: "---", "-", ".", or space
    if (str[0] == '-' || str[0] == '.' || str[0] == ' ')
        return REST;
    
    // Parse note name
    int noteBase = -1;
    int sharp = 0;
    
    switch (str[0])
    {
        case 'C': case 'c': noteBase = 0; break;
        case 'D': case 'd': noteBase = 2; break;
        case 'E': case 'e': noteBase = 4; break;
        case 'F': case 'f': noteBase = 5; break;
        case 'G': case 'g': noteBase = 7; break;
        case 'A': case 'a': noteBase = 9; break;
        case 'B': case 'b': noteBase = 11; break;
        default: return 0; // Invalid
    }
    
    // Check for sharp
    int octavePos = 1;
    if (str[1] == '#' || str[1] == 's')
    {
        sharp = 1;
        octavePos = 2;
    }
    else if (str[1] == '-')
    {
        octavePos = 2;
    }
    
    // Parse octave (0-7)
    int octave = str[octavePos] - '0';
    if (octave < 0 || octave > 7)
        return 0; // Invalid
    
    // Calculate note value: FIRSTNOTE + (octave * 12) + noteBase + sharp
    int noteValue = FIRSTNOTE + (octave * 12) + noteBase + sharp;
    
    // Check bounds
    if (noteValue < FIRSTNOTE || noteValue > LASTNOTE)
        return REST;
    
    return (unsigned char)noteValue;
}

// Export marked pattern notes to clipboard as text
// Format: "C-4 D#5 --- F#3 | C-4 --- G-2 A-5 | ..."
// Each channel separated by " | ", notes separated by space
void exportPatternNotesToClipboard(GTOBJECT *gt, int c2)
{
    if (editorInfo.epmarkchn == -1)
    {
        sprintf(infoTextBuffer, "No pattern marked!");
        forceInfoLine = 1;
        return;
    }
    
    int mc2 = getActualChannel(editorInfo.esnum, editorInfo.epmarkchn);
    int start = editorInfo.epmarkstart;
    int end = editorInfo.epmarkend;
    
    if (editorInfo.epmarkstart >= editorInfo.epmarkend)
    {
        start = end;
        end = editorInfo.epmarkstart;
    }
    
    // Allocate buffer for text (estimate: 4 chars per note + space + margin)
    int rowCount = end - start + 1;
    char *textBuffer = (char*)malloc(rowCount * 6 + 100);
    if (!textBuffer)
    {
        sprintf(infoTextBuffer, "Out of memory!");
        forceInfoLine = 1;
        return;
    }
    
    textBuffer[0] = '\0';
    
    // Export notes with measure separators
    int rowIndex = 0; // Track position for measure separators
    for (int row = start; row <= end; row++)
    {
        if (row >= pattlen[gt->editorUndoInfo.editorInfo[mc2].epnum])
            break;
        
        unsigned char note = pattern[gt->editorUndoInfo.editorInfo[mc2].epnum][row * 4];
        
        if (note == REST || note < FIRSTNOTE || note > LASTNOTE)
        {
            strcat(textBuffer, "---");
        }
        else
        {
            int noteIndex = note - FIRSTNOTE;
            strcat(textBuffer, notename[noteIndex]);
        }
        
        // Add separator (space or measure separator)
        if (row < end)
        {
            rowIndex++;
            // Add measure separator (|) at stepsize boundaries
            if ((rowIndex % stepsize) == 0)
                strcat(textBuffer, " | ");
            else
                strcat(textBuffer, " ");
        }
    }
    
    // Copy to clipboard
    if (SDL_SetClipboardText(textBuffer) == 0)
    {
        sprintf(infoTextBuffer, "Copied %d notes to clipboard", rowCount);
        forceInfoLine = 1;
    }
    else
    {
        sprintf(infoTextBuffer, "Clipboard copy failed!");
        forceInfoLine = 1;
    }
    
    free(textBuffer);
}

// Import pattern notes from clipboard text
// Supports formats: "C-4 D#5 --- F#3" or "C4 D#5 - F#3" 
void importPatternNotesFromClipboard(GTOBJECT *gt, int c2)
{
    if (!SDL_HasClipboardText())
    {
        sprintf(infoTextBuffer, "Clipboard is empty!");
        forceInfoLine = 1;
        return;
    }
    
    char *clipText = SDL_GetClipboardText();
    if (!clipText || clipText[0] == '\0')
    {
        if (clipText)
            SDL_free(clipText);
        sprintf(infoTextBuffer, "Clipboard is empty!");
        forceInfoLine = 1;
        return;
    }
    
    // Parse notes from clipboard
    int noteCount = 0;
    unsigned char notes[MAX_PATTROWS];
    char *ptr = clipText;
    char noteStr[10];
    int noteStrPos = 0;
    
    while (*ptr && noteCount < MAX_PATTROWS)
    {
        char ch = *ptr;
        
        if (ch == ' ' || ch == '|' || ch == ',' || ch == '\t' || ch == '\n' || ch == '\r')
        {
            // End of note token
            if (noteStrPos > 0)
            {
                noteStr[noteStrPos] = '\0';
                notes[noteCount] = parseNoteString(noteStr);
                noteCount++;
                noteStrPos = 0;
            }
        }
        else
        {
            // Accumulate note characters
            if (noteStrPos < 9)
            {
                noteStr[noteStrPos++] = ch;
            }
        }
        
        ptr++;
    }
    
    // Handle last note if any
    if (noteStrPos > 0)
    {
        noteStr[noteStrPos] = '\0';
        notes[noteCount] = parseNoteString(noteStr);
        noteCount++;
    }
    
    SDL_free(clipText);
    
    if (noteCount == 0)
    {
        sprintf(infoTextBuffer, "No valid notes in clipboard!");
        forceInfoLine = 1;
        return;
    }
    
    // Paste notes starting at current position
    int row = editorInfo.eppos;
    int pattNum = gt->editorUndoInfo.editorInfo[c2].epnum;
    int pasted = 0;
    
    for (int i = 0; i < noteCount && row < pattlen[pattNum]; i++, row++)
    {
        if (notes[i] != 0) // Valid note
        {
            pattern[pattNum][row * 4] = notes[i];
            pasted++;
        }
    }
    
    sprintf(infoTextBuffer, "Pasted %d notes from clipboard", pasted);
    forceInfoLine = 1;
}
