class CaperBridge(object):
    """Mimic a pygr.cnestedlist.NLMSA with a caper database."""

    def __init__(self, caper_map, genome_db, reads_db):
        self.map = caper_map
        self.genome_db = genome_db
        self.reads_db = reads_db

    def __getitem__(self, ival):
        seq_id, start, stop = ival.id, ival.start, ival.stop

        return CaperSlice(self, ival, self.map.get_reads_at(seq_id, start, stop))

class CaperSlice(object):
    def __init__(self, bridge, ival, m):
        self.ival = ival
        self.bridge = bridge
        self.m = m

    def __iter__(self):
        db = self.bridge.reads_db
        for ival_start, ival_stop, match_name, start, stop, o in self.m:
            if ival_start == ival_stop:
                continue

            if o == -1:
                read = -db[match_name]
            else:
                read = db[match_name]

            if start > stop:
                start, stop = stop - 1, start - 1

            yield read[start:stop]
                

    def keys(self):
        return list(iter(self))

    def edges(self):
        ival = self.ival
        db = self.bridge.reads_db
        for ival_start, ival_stop, match_name, start, stop, o in self.m:
            if ival_start == ival_stop:
                continue

            yield ival.pathForward[ival_start:ival_stop], \
                  db[match_name][start:stop], \
                  None

    def __len__(self):
        return len(self.m)
