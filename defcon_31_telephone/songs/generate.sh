function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}
COUNTER=123 # start of the custom tracks
TRACKS_ARR=();
> songs.txt
for file in *.mp3;
do
   PADDED_COUNTER=$(printf "%03d" $COUNTER);
   echo $file;
   echo ${file%.*}
   TRACK_NAME="TRACK_$(echo ${file%.*} | iconv -c -t ascii//TRANSLIT | sed -r s/[^a-zA-Z0-9]+/_/g | sed -r s/^-+\|-+$//g | tr a-z A-Z | sed s/_$//)";
   TRACKS_ARR+=(${TRACK_NAME});
   echo "const char ${TRACK_NAME}[] PROGMEM = \"${PADDED_COUNTER}\";" | tee -a songs.txt;
   FILENAME="track${PADDED_COUNTER}.mp3";
   sox "$file" --norm=-0.1 -C 192 -c 1 "converted/$FILENAME" -V;
   ((COUNTER++));
done
echo "const char* const RANDOM_TRACKS[] PROGMEM = { $(join_by ", " "${TRACKS_ARR[@]}") };" | tee -a songs.txt

# Define the start and end markers
START="// START OF GENERATED SECTION"
END="// END OF GENERATED SECTION"

# Define the input and output files
INPUT="songs.txt"
OUTPUT="../defcon_telephone.ino"

# Create a temporary file
TEMP=$(mktemp)

# Use awk to process the file
awk -v input="$INPUT" -v start="$START" -v end="$END" '
  $0 == start {print; system("cat " input); flag=1; next}
  $0 == end {flag=0}
  !flag' $OUTPUT > $TEMP

# Move the temporary file to the output file
mv $TEMP $OUTPUT
