//
// GTUltra Clipboard Functions Header
//

#ifndef GCLIPBOARD_H
#define GCLIPBOARD_H

// Export marked pattern notes to clipboard as text
void exportPatternNotesToClipboard(GTOBJECT *gt, int c2);

// Import pattern notes from clipboard text
void importPatternNotesFromClipboard(GTOBJECT *gt, int c2);

#endif // GCLIPBOARD_H
