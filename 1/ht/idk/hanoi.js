const hanoi = (n, start, end) => {
    if (n == 1) {
        console.log ("T"+start + " => " + "T"+end);
        return;
    };
    let mid = 6 - start - end;
    hanoi (n-1, start, mid);
    hanoi (1, start, end);
    hanoi (n-1, mid, end);
    return;
};

hanoi (3, 1, 3);