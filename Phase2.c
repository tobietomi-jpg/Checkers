#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint64_t SetBit64(uint64_t value1, int pos1){
    if (pos1 < 0 || pos1 >= 64) return value1;
    return value1 | (1ull << pos1);
}

uint64_t ClearBit64(uint64_t value1, int pos1){
    if(pos1 < 0 || pos1 >= 64) return value1;
    return value1 & ~(1ull << pos1);
}

uint64_t ToggleBit64(uint64_t value1, int pos1){
    if(pos1 < 0 || pos1 >= 64) return value1;
    return value1 ^ (1ull << pos1);
}

int GetBit64(uint64_t value, int pos){
    if(pos < 0 || pos >= 64) return 0;
    return (int)((value >> pos) & 1ull);
}

int CountBits64(uint64_t value){
    int count = 0;
    while (value){
        value &= (value - 1);
        count++;
    }
    return count;
}


int sq32_to_bit64[33];
int bit64_to_sq32[64];

void build_mappings(void){
    for (int i = 0; i < 64; i++) bit64_to_sq32[i] = 0;
    int n = 1;
    for(int row = 0; row < 8; row++){
        for(int col = 0; col < 8; col++){
            if((row + col) % 2 == 1){
                int bit = row * 8 + col;
                sq32_to_bit64[n] = bit;
                bit64_to_sq32[bit] = n;
                n++;
            }
        }
    }
}

void bit_to_rc(int bit, int *row, int *col){
    *row = bit / 8;
    *col = bit % 8;
}

int rc_to_bit(int row, int col){
    return row * 8 + col;
}

typedef struct{
    uint64_t p1_pieces;
    uint64_t p1_kings;
    uint64_t p2_pieces;
    uint64_t p2_kings;
    int current_turn;
} GameState;

void Init(GameState *g){
    g->p1_pieces = 0ull;
    g->p1_kings = 0ull;
    g->p2_pieces = 0ull;
    g->p2_kings = 0ull;
    for(int sq = 1; sq <= 12; sq++){
        int bit = sq32_to_bit64[sq];
        g->p1_pieces = SetBit64(g->p1_pieces, bit);
    }
    for(int sq = 21; sq <= 32; sq++){
        int bit = sq32_to_bit64[sq];
        g->p2_pieces = SetBit64(g->p2_pieces, bit);
    }
    g->current_turn = 1;
}

int IsOccupied(const GameState *g, int bit){
    uint64_t mask = (1ull << bit);
    return ((g->p1_pieces | g->p1_kings | g->p2_pieces | g->p2_kings) & mask) != 0;
}

int IsPlayerPiece(const GameState *g, int player, int bit){
    uint64_t mask = (1ull << bit);
    if(player == 1) return ((g->p1_pieces | g->p1_kings) & mask) != 0;
    return ((g->p2_pieces | g->p2_kings) & mask) != 0;
}

int IsKing(const GameState *g, int player, int bit){
    uint64_t mask = (1ull << bit);
    if(player == 1) return (g->p1_kings & mask) != 0;
    return (g->p2_kings & mask) != 0;
}

void MakeKing(GameState *g, int player, int bit){
    uint64_t mask = (1ull << bit);
    if (player == 1){
        g->p1_pieces &= ~mask;
        g->p1_kings |= mask;
    }
    else{
        g->p2_pieces &= ~mask;
        g->p2_kings |= mask;
    }
}

void RemoveAnyPiece(GameState *g, int bit){
    uint64_t mask = ~(1ull << bit);
    g->p1_pieces &= mask;
    g->p1_kings &= mask;
    g->p2_pieces &= mask;
    g->p2_kings &= mask;
}

void PlacePiece(GameState *g, int player, int bit, int king){
    uint64_t mask = (1ull << bit);
    RemoveAnyPiece(g, bit);
    if(player == 1){
        if (king) g->p1_kings |= mask;
        else g->p1_pieces |= mask;
    }
    else{
        if(king) g->p2_kings |= mask;
        else g->p2_pieces |= mask;
    }
}

void PrintBoard(const GameState *g){
    puts("\n   +---------------------------------+");
    for(int r = 0; r < 8; r++){
        printf(" %d |", 8 - r);
        for(int c = 0; c < 8; c++){
            int bit = rc_to_bit(r,c);
            if((r + c) % 2 == 1){
                int sq = bit64_to_sq32[bit];
                if(IsPlayerPiece(g, 1, bit)){
                    if(IsKing(g, 1, bit)) printf("B");
                    else printf("b");
                }else if (IsPlayerPiece(g, 2, bit)){
                    if(IsKing(g,2,bit)) printf("W");
                    else printf("w");
                }else{
                    if(sq>0){
                        if(sq < 10) printf("%d", sq);
                        else printf("%d", sq);
                    }
                    else{
                        printf(" . ");
                    }
                }
            }else{
                printf(" . ");
            }
        }
        printf("|\n");
    }
    puts("   +---------------------------------+");
    puts("     a  b  c  d  e  f  g  h");
    printf("Player 1 pieces: %d (kings %d)  Player 2 pieces: %d (kings %d)\n",
        CountBits64(g->p1_pieces), CountBits64(g->p1_kings),
        CountBits64(g->p2_pieces), CountBits64(g->p2_kings));
}

int is_simple_move_legal(const GameState *g, int player, int bit, int to){
    if(IsOccupied(g, to)) return 0;
    int r0,c0,r1,c1;
    bit_to_rc(bit, &r0, &c0);
    bit_to_rc(to, &r1, &c1);
    int dr = r1 - r0;
    int dc = c1 - c0;
    if (abs(dr) != 1 || abs(dc) != 1) return 0;
    if (IsKing(g, player, bit)) return 1;
    if (player == 1 && dr == 1) return 1; // player1 moves down (increasing row)
    if (player == 2 && dr == -1) return 1; // player2 moves up
    return 0;
}

int is_capture_legal(const GameState *g, int player, int bit, int over, int land){
    if (!IsOccupied(g,over)) return 0;
    if(IsPlayerPiece(g, player, over)) return 0;
    if(IsOccupied(g,land)) return 0;
    int r0,c0,ro,co,rl,cl;
    bit_to_rc(bit, &r0, &c0);
    bit_to_rc(over, &ro, &co);
    bit_to_rc(land, &rl, &cl);
    int dr_over = ro - r0;
    int dc_over = co - c0;
    int dr_land = rl - r0;
    int dc_land = cl - c0;

    if(abs(dr_over) != 1 || abs(dc_over) !=1) return 0;
    if(abs(dr_land) != 2 || abs(dc_land) != 2) return 0;
    if (dr_land != 2*dr_over || dc_land != 2*dc_over) return 0;
    
    if (IsKing(g, player, bit)) return 1;
    if (player == 1 && dr_over == 1) return 1;
    if (player == 2 && dr_over == -1) return 1;
    return 0;
}

#define MAX_SEQ 64
typedef struct {
    int seq_len;
    int seq[MAX_SEQ]; // seq[0] is start (bit), seq[1] first landing, ...
} CaptureSeq;

void find_captures_from(GameState gcopy,  int player, int curbit, CaptureSeq *curseq, CaptureSeq results[], int *rescount){
    int found_any = 0;
    int r0,c0;
    bit_to_rc(curbit, &r0, &c0);
    int drs[4]  ={1,1,-1,-1};
    int dcs[4] = {1,-1,1,-1};
    int was_king = IsKing(&gcopy, player, curbit);
    for(int d = 0; d < 4; d++){
        int ro = r0 + drs[d];
        int co = c0 + dcs[d];
        int rl = r0 + 2*drs[d];
        int cl = c0 + 2*dcs[d];
        if(ro < 0 || ro >= 8 || co < 0 || co >= 8) continue;
        if (rl < 0 || rl >= 8 || cl < 0 || cl >= 8) continue;
        int over = rc_to_bit(ro,co);
        int land = rc_to_bit(rl,cl);

        if(is_capture_legal(&gcopy, player, curbit, over, land)){
            GameState next = gcopy;
            RemoveAnyPiece(&next, curbit);
            RemoveAnyPiece(&next, over);
            PlacePiece(&next, player, land, was_king);
            if (curseq->seq_len < MAX_SEQ) {
                curseq->seq[curseq->seq_len++] = land;
                find_captures_from(next, player, land, curseq, results, rescount);
                curseq->seq_len--;
            }
            found_any = 1;
        }
    }
    if (!found_any){
        if(curseq->seq_len > 1){
            if(*rescount < 256){
                results[*rescount] = *curseq;
                (*rescount)++;
            }
        }
    }
}

int GetAllCaptures(const GameState *g,  int player, CaptureSeq results[], int *outcount){
    *outcount = 0;
    uint64_t all = (player == 1) ? (g->p1_pieces | g->p1_kings) : (g->p2_pieces | g->p2_kings);
    for (int bit = 0; bit < 64; ++bit) {
        if (!GetBit64(all, bit)) continue;
        CaptureSeq cur;
        cur.seq_len = 0;
        cur.seq[cur.seq_len++] = bit;
        GameState gcopy = *g;
        find_captures_from(gcopy, player, bit, &cur, results, outcount);
    }
    return *outcount;
}

int ApplyCaptureSeq(GameState *g, int player, CaptureSeq *seq){
    if (seq->seq_len < 2) return 0;
    int start = seq->seq[0];
    int cur = start;
    int was_king = IsKing(g, player, start);
    RemoveAnyPiece(g, start);
    for (int i = 1; i < seq->seq_len; ++i) {
        int land = seq->seq[i];
        int r0,c0,rl,cl,ro,co;
        bit_to_rc(cur,&r0,&c0);
        bit_to_rc(land,&rl,&cl);
        ro = (r0 + rl) / 2;
        co = (c0 + cl) / 2;
        int over = rc_to_bit(ro,co);
        RemoveAnyPiece(g, over);
        cur = land;
    }
    int final_row, final_col;
    bit_to_rc(cur, &final_row, &final_col);
    int promote = 0;
    if (!was_king){
        if(player == 1 && final_row == 7) promote = 1;
        if(player == 2 && final_row == 0) promote = 1;
    }
    PlacePiece(g, player, cur, was_king || promote);
    return 1;
}

int ApplySimpleMove(GameState *g, int player, int from, int to) {
    if (!is_simple_move_legal(g, player, from, to)) return 0;
    int was_king = IsKing(g, player, from);
    RemoveAnyPiece(g, from);
    int row,to_col;
    bit_to_rc(to, &row, &to_col);
    int promote = 0;
    if (!was_king) {
        if (player == 1 && row == 7) promote = 1;
        if (player == 2 && row == 0) promote = 1;
    }
    PlacePiece(g, player, to, was_king || promote);
    return 1;
}

int parse_move_string(const char *s, int *from, int *to, int *is_capture) {
    while (*s == ' ') s++;
    char buf[128];
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = 0;
    *is_capture = 0;
    char *p = buf;
    while (*p) {
        if (*p == 'x' || *p == 'X') { *p = '-'; *is_capture = 1; }
        p++;
    }
    int a = 0, b = 0;
    if (sscanf(buf, "%d-%d", &a, &b) == 2) {
        *from = a; *to = b;
        return 1;
    }
    if (sscanf(buf, "%d %d", &a, &b) == 2) {
        *from = a; *to = b;
        return 1;
    }
    return 0;
}

int parse_capture_sequence(const char *s, int seq_out[], int *seq_len_out) {
    char buf[128];
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = 0;
    for (int i = 0; buf[i]; ++i) {
        if ((buf[i] >= '0' && buf[i] <= '9') || buf[i] == ' ') continue;
        if (buf[i] == 'x' || buf[i] == 'X' || buf[i] == '-') buf[i] = ' ';
        else buf[i] = ' ';
    }
    int vals[32], cnt = 0;
    char *tok = strtok(buf, " ");
    while (tok && cnt < 32) {
        vals[cnt++] = atoi(tok);
        tok = strtok(NULL, " ");
    }
    if (cnt >= 2) {
        for (int i = 0; i < cnt; ++i) seq_out[i] = vals[i];
        *seq_len_out = cnt;
        return 1;
    }
    return 0;
}

int AnyCaptureExists(const GameState *g, int player) {
    CaptureSeq results[256];
    int count = 0;
    GetAllCaptures(g, player, results, &count);
    return count > 0;
}

int TryApplyMoveFromString(GameState *g, const char *input) {
    int from,to,is_cap;
    if (!parse_move_string(input, &from, &to, &is_cap)) {
        int seq_nums[32], seq_len=0;
        if (parse_capture_sequence(input, seq_nums, &seq_len)) {
            if (seq_len < 2) return 0;
            CaptureSeq seq;
            seq.seq_len = 0;
            for (int i = 0; i < seq_len; ++i) {
                int sq = seq_nums[i];
                if (sq < 1 || sq > 32) return 0;
                seq.seq[seq.seq_len++] = sq32_to_bit64[sq];
            }
            if (seq.seq_len < 2) return 0;
            // quick validation (same as before)
            int player = g->current_turn;
            int start = seq.seq[0];
            if (!IsPlayerPiece(g, player, start)) return 0;
            GameState gcopy = *g;
            for (int i = 1; i < seq.seq_len; ++i) {
                int land = seq.seq[i];
                int r0,c0,rl,cl,ro,co;
                bit_to_rc(start,&r0,&c0);
                bit_to_rc(land,&rl,&cl);
                ro = (r0 + rl)/2; co = (c0 + cl)/2;
                int over = rc_to_bit(ro,co);
                if (!is_capture_legal(&gcopy, player, start, over, land)) return 0;
                RemoveAnyPiece(&gcopy, over);
                RemoveAnyPiece(&gcopy, start);
                int was_king = IsKing(&gcopy, player, start);
                PlacePiece(&gcopy, player, land, was_king);
                start = land;
            }
            ApplyCaptureSeq(g, g->current_turn, &seq);
            return 1;
        }
        return 0;
    }
    if (from < 1 || from > 32 || to < 1 || to > 32) return 0;
    int frombit = sq32_to_bit64[from];
    int tobit = sq32_to_bit64[to];
    int player = g->current_turn;
    if (!IsPlayerPiece(g, player, frombit)) return 0;
    if (AnyCaptureExists(g, player)) {
        printf("A capture is available — you must capture. Use x and provide full capture sequence if possible.\n");
        return 0;
    }
    if (ApplySimpleMove(g, player, frombit, tobit)) return 1;
    return 0;
}

int HasPlayerWon(const GameState *g, int player) {
    int opp = (player == 1) ? 2 : 1;
    uint64_t opp_all = (opp == 1) ? (g->p1_pieces | g->p1_kings) : (g->p2_pieces | g->p2_kings);
    return (opp_all == 0ull);
}

void game_loop(GameState *g) {
    char input[128];
    while (1) {
        PrintBoard(g);
        int player = g->current_turn;
        printf("Player %d's turn (%s). ", player, player==1 ? "b (moves down)" : "w (moves up)");
        if (AnyCaptureExists(g, player)) printf("Capture available! ");
        printf("Enter move (e.g. 12-16 or 12x19x28), or 'q' to quit: ");
        if (!fgets(input, sizeof(input), stdin)) break;
        size_t L = strlen(input);
        if (L && input[L-1] == '\n') input[L-1] = 0;
        if (strcmp(input, "q") == 0) break;
        if (strlen(input) == 0) continue;
        if (!TryApplyMoveFromString(g, input)) {
            printf("Invalid move or format. Try again.\n");
            continue;
        }
        if (HasPlayerWon(g, player)) {
            PrintBoard(g);
            printf("Player %d wins! Congratulations.\n", player);
            break;
        }
        g->current_turn = (player == 1) ? 2 : 1;
    }
}

int main(void) {
    build_mappings();
    GameState game;
    Init(&game);
    puts("Welcome to Bitboard Checkers (text mode).");
    puts("Numbering: playable squares are labeled 1..32 on the board.");
    puts("Player 1 (b) starts at the top and moves down. Player 2 (w) starts at bottom and moves up.");
    puts("Notation: 12-16 simple move, 12x19 capture (you may chain captures: 12x19x28).");
    game_loop(&game);
    puts("Goodbye.");
    return 0;
}
