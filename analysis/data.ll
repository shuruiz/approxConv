; ModuleID = 'data.c'
source_filename = "data.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }

@outH = dso_local constant i32 996, align 4
@outW = dso_local constant i32 996, align 4
@__const.testloops.array = private unnamed_addr constant [5 x [5 x float]] [[5 x float] [float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01], [5 x float] [float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01], [5 x float] [float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01], [5 x float] [float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01, float 1.000000e+01], [5 x float] [float 1.000000e+02, float 1.000000e+02, float 1.000000e+02, float 1.000000e+02, float 1.000000e+02]], align 16
@stdout = external dso_local global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str.1 = private unnamed_addr constant [3 x i8] c"%f\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @testloops(float* %input, float* %output) #0 {
entry:
  %input.addr = alloca float*, align 8
  %output.addr = alloca float*, align 8
  %array = alloca [5 x [5 x float]], align 16
  %oh = alloca i32, align 4
  %ow = alloca i32, align 4
  %kh = alloca i32, align 4
  %kw = alloca i32, align 4
  store float* %input, float** %input.addr, align 8
  store float* %output, float** %output.addr, align 8
  %0 = bitcast [5 x [5 x float]]* %array to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %0, i8* align 16 bitcast ([5 x [5 x float]]* @__const.testloops.array to i8*), i64 100, i1 false)
  store i32 0, i32* %oh, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc28, %entry
  %1 = load i32, i32* %oh, align 4
  %cmp = icmp slt i32 %1, 996
  br i1 %cmp, label %for.body, label %for.end30

for.body:                                         ; preds = %for.cond
  store i32 0, i32* %ow, align 4
  br label %for.cond1

for.cond1:                                        ; preds = %for.inc25, %for.body
  %2 = load i32, i32* %ow, align 4
  %cmp2 = icmp slt i32 %2, 996
  br i1 %cmp2, label %for.body3, label %for.end27

for.body3:                                        ; preds = %for.cond1
  store i32 0, i32* %kh, align 4
  br label %for.cond4

for.cond4:                                        ; preds = %for.inc22, %for.body3
  %3 = load i32, i32* %kh, align 4
  %cmp5 = icmp slt i32 %3, 5
  br i1 %cmp5, label %for.body6, label %for.end24

for.body6:                                        ; preds = %for.cond4
  store i32 0, i32* %kw, align 4
  br label %for.cond7

for.cond7:                                        ; preds = %for.inc, %for.body6
  %4 = load i32, i32* %kw, align 4
  %cmp8 = icmp slt i32 %4, 5
  br i1 %cmp8, label %for.body9, label %for.end

for.body9:                                        ; preds = %for.cond7
  %5 = load float*, float** %input.addr, align 8
  %6 = load i32, i32* %oh, align 4
  %7 = load i32, i32* %kh, align 4
  %add = add nsw i32 %6, %7
  %mul = mul nsw i32 %add, 1000
  %8 = load i32, i32* %ow, align 4
  %add10 = add nsw i32 %mul, %8
  %9 = load i32, i32* %kw, align 4
  %add11 = add nsw i32 %add10, %9
  %idxprom = sext i32 %add11 to i64
  %arrayidx = getelementptr inbounds float, float* %5, i64 %idxprom
  %10 = load float, float* %arrayidx, align 4
  %11 = load i32, i32* %kh, align 4
  %idxprom12 = sext i32 %11 to i64
  %arrayidx13 = getelementptr inbounds [5 x [5 x float]], [5 x [5 x float]]* %array, i64 0, i64 %idxprom12
  %12 = load i32, i32* %kw, align 4
  %idxprom14 = sext i32 %12 to i64
  %arrayidx15 = getelementptr inbounds [5 x float], [5 x float]* %arrayidx13, i64 0, i64 %idxprom14
  %13 = load float, float* %arrayidx15, align 4
  %mul16 = fmul float %10, %13
  %14 = load float*, float** %output.addr, align 8
  %15 = load i32, i32* %oh, align 4
  %mul17 = mul nsw i32 %15, 996
  %16 = load i32, i32* %ow, align 4
  %add18 = add nsw i32 %mul17, %16
  %idxprom19 = sext i32 %add18 to i64
  %arrayidx20 = getelementptr inbounds float, float* %14, i64 %idxprom19
  %17 = load float, float* %arrayidx20, align 4
  %add21 = fadd float %17, %mul16
  store float %add21, float* %arrayidx20, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body9
  %18 = load i32, i32* %kw, align 4
  %inc = add nsw i32 %18, 1
  store i32 %inc, i32* %kw, align 4
  br label %for.cond7

for.end:                                          ; preds = %for.cond7
  br label %for.inc22

for.inc22:                                        ; preds = %for.end
  %19 = load i32, i32* %kh, align 4
  %inc23 = add nsw i32 %19, 1
  store i32 %inc23, i32* %kh, align 4
  br label %for.cond4

for.end24:                                        ; preds = %for.cond4
  br label %for.inc25

for.inc25:                                        ; preds = %for.end24
  %20 = load i32, i32* %ow, align 4
  %inc26 = add nsw i32 %20, 1
  store i32 %inc26, i32* %ow, align 4
  br label %for.cond1

for.end27:                                        ; preds = %for.cond1
  br label %for.inc28

for.inc28:                                        ; preds = %for.end27
  %21 = load i32, i32* %oh, align 4
  %inc29 = add nsw i32 %21, 1
  store i32 %inc29, i32* %oh, align 4
  br label %for.cond

for.end30:                                        ; preds = %for.cond
  %22 = load %struct._IO_FILE*, %struct._IO_FILE** @stdout, align 8
  %23 = load float*, float** %output.addr, align 8
  %arrayidx31 = getelementptr inbounds float, float* %23, i64 0
  %24 = load float, float* %arrayidx31, align 4
  %conv = fpext float %24 to double
  %call = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %22, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), double %conv)
  ret void
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

declare dso_local i32 @fprintf(%struct._IO_FILE*, i8*, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %input = alloca [1000000 x float], align 16
  %output = alloca [992016 x float], align 16
  %i = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %0 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %0, 1000000
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %arraydecay = getelementptr inbounds [1000000 x float], [1000000 x float]* %input, i64 0, i64 0
  %1 = load i32, i32* %i, align 4
  %idx.ext = sext i32 %1 to i64
  %add.ptr = getelementptr inbounds float, float* %arraydecay, i64 %idx.ext
  %call = call i32 (i8*, ...) @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.1, i64 0, i64 0), float* %add.ptr)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %2 = load i32, i32* %i, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, i32* %i, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %arraydecay1 = getelementptr inbounds [1000000 x float], [1000000 x float]* %input, i64 0, i64 0
  %arraydecay2 = getelementptr inbounds [992016 x float], [992016 x float]* %output, i64 0, i64 0
  call void @testloops(float* %arraydecay1, float* %arraydecay2)
  ret i32 0
}

declare dso_local i32 @__isoc99_scanf(i8*, ...) #2

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0 "}
